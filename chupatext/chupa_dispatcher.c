/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include <chupatext/config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>

#include "chupa_dispatcher.h"
#include "chupa_module_factory.h"
#include "chupa_module_factory_utils.h"
#include "chupa_module_description.h"

#define CHUPA_DISPATCHER_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                    \
                                 CHUPA_TYPE_DISPATCHER,    \
                                 ChupaDispatcherPrivate))

typedef struct _ChupaDispatcherPrivate    ChupaDispatcherPrivate;
struct _ChupaDispatcherPrivate
{
    GList *descriptions;
};

G_DEFINE_TYPE(ChupaDispatcher, chupa_dispatcher, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void
chupa_dispatcher_class_init (ChupaDispatcherClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(ChupaDispatcherPrivate));
}

static void
load_module_descriptions (ChupaDispatcherPrivate *priv)
{
    GDir *dir;
    GSList *sorted_paths = NULL;
    GSList *node = NULL;
    const gchar *descriptions_path, *entry;

    descriptions_path = g_getenv("CHUPA_DESCRIPTIONS_DIR");
    if (!descriptions_path)
        descriptions_path = DESCRIPTIONS_DIR;
    dir = g_dir_open(descriptions_path, 0, NULL);
    if (!dir)
        return;

    while ((entry = g_dir_read_name(dir))) {
        if (!g_str_has_suffix(entry, ".desc"))
            continue;
        sorted_paths = g_slist_prepend(sorted_paths,
                                       g_build_filename(descriptions_path,
                                                        entry,
                                                        NULL));
    }
    sorted_paths = g_slist_sort(sorted_paths, (GCompareFunc)strcmp);

    for (node = sorted_paths; node; node = g_slist_next(node)) {
        ChupaModuleDescription *description;
        GKeyFile *key_file;
        const gchar *path;
        gchar **groups, **group;
        GError *error = NULL;

        path = node->data;
        key_file = g_key_file_new();
        if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error)) {
            g_warning("%s", error->message);
            g_error_free(error);
            g_key_file_free(key_file);
            continue;
        }

        groups = g_key_file_get_groups(key_file, NULL);
        for (group = groups; *group; group++) {
            gchar *name, **mime_types, **mime_type;

            error = NULL;
            name = g_key_file_get_string(key_file, *group, "name", &error);
            if (!name) {
                g_warning("%s", error->message);
                g_error_free(error);
                continue;
            }
            description = chupa_module_description_new(name);
            g_free(name);
            chupa_module_description_set_label(description, *group);
            mime_types = g_key_file_get_string_list(key_file, *group,
                                                    "mime_types", NULL, &error);
            if (error) {
                g_warning("%s", error->message);
                g_error_free(error);
                continue;
            }
            for (mime_type = mime_types; *mime_type; mime_type++) {
                chupa_module_description_add_mime_type(description, *mime_type);
            }
            g_strfreev(mime_types);
            priv->descriptions = g_list_append(priv->descriptions, description);
        }
        g_strfreev(groups);
        g_key_file_free(key_file);
    }
    g_slist_foreach(sorted_paths, (GFunc)g_free, NULL);
    g_slist_free(sorted_paths);
    g_dir_close(dir);
}

static void
chupa_dispatcher_init (ChupaDispatcher *dispatcher)
{
    ChupaDispatcherPrivate *priv;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(dispatcher);
    priv->descriptions = NULL;
    load_module_descriptions(priv);
}

static void
dispose(GObject *object)
{
    ChupaDispatcherPrivate *priv;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(object);
    if (priv->descriptions) {
        g_list_foreach(priv->descriptions, (GFunc)g_object_unref, NULL);
        g_list_free(priv->descriptions);
        priv->descriptions = NULL;
    }

    G_OBJECT_CLASS(chupa_dispatcher_parent_class)->dispose(object);
}

ChupaDispatcher *
chupa_dispatcher_new(void)
{
    return g_object_new(CHUPA_TYPE_DISPATCHER, NULL);
}

ChupaDecomposer *
chupa_dispatcher_dispatch(ChupaDispatcher *dispatcher, const gchar *mime_type)
{
    ChupaDispatcherPrivate *priv;
    GList *node;
    ChupaModuleFactory *factory = NULL;
    GString *normalized_type = NULL;
    gboolean normalized = FALSE;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(dispatcher);
    for (node = priv->descriptions; !factory && node; node = g_list_next(node)) {
        ChupaModuleDescription *description = node->data;
        GList *mime_types;
        const gchar *sub_type;

        mime_types = chupa_module_description_get_mime_types(description);
        for (; !factory && mime_types; mime_types = g_list_next(mime_types)) {
            if (strcmp(mime_type, mime_types->data) == 0) {
                factory = chupa_module_description_get_factory(description);
            } else {
                if (!normalized_type) {
                    if (normalized)
                        continue;
                    normalized = TRUE;
                    sub_type = strchr(mime_type, '/');
                    if (!sub_type)
                        continue;
                    sub_type++;
                    if (!g_str_has_prefix(sub_type, "x-"))
                        continue;

                    normalized_type = g_string_new_len(mime_type,
                                                       sub_type - mime_type);
                    g_string_append(normalized_type, sub_type + 2);

                    if (!normalized_type)
                        continue;
                }
                if (strcmp(normalized_type->str, mime_types->data) == 0) {
                    factory = chupa_module_description_get_factory(description);
                }
            }
        }
    }

    if (normalized_type)
        g_string_free(normalized_type, TRUE);

    if (!factory)
        return NULL;

    return CHUPA_DECOMPOSER(chupa_module_factory_create(factory));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
