/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>

#include <chupatext.h>
#include "chupa_decomposer_description.h"
#include "chupa_utils.h"

static GList *descriptions = NULL;

static void dispose_factory(ChupaDecomposerDescription *description);

static void
load_decomposer_description (const gchar *path)
{
    ChupaDecomposerDescription *description;
    GKeyFile *key_file;
    gchar **groups, **group;
    GError *error = NULL;

    key_file = g_key_file_new();
    if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error)) {
        g_warning("%s", error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        return;
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
        description = chupa_decomposer_description_new(name);
        g_free(name);
        chupa_decomposer_description_set_label(description, *group);
        mime_types = g_key_file_get_string_list(key_file, *group,
                                                "mime_types", NULL, &error);
        if (error) {
            g_warning("%s", error->message);
            g_error_free(error);
            continue;
        }
        for (mime_type = mime_types; *mime_type; mime_type++) {
            chupa_decomposer_description_add_mime_type(description, *mime_type);
        }
        g_strfreev(mime_types);
        descriptions = g_list_append(descriptions, description);
    }
    g_strfreev(groups);
    g_key_file_free(key_file);
}

static void
load_decomposer_descriptions (void)
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
        load_decomposer_description(node->data);
    }
    g_slist_foreach(sorted_paths, (GFunc)g_free, NULL);
    g_slist_free(sorted_paths);
    g_dir_close(dir);
}

void
chupa_decomposer_description_init (void)
{
    descriptions = NULL;
    load_decomposer_descriptions();
}

void
chupa_decomposer_description_quit (void)
{
    g_list_foreach(descriptions, (GFunc)dispose_factory, NULL);
    g_list_foreach(descriptions, (GFunc)g_object_unref, NULL);
    g_list_free(descriptions);
    descriptions = NULL;
}

GList *
chupa_decomposer_description_get_list (void)
{
    return descriptions;
}

#define CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(obj)                   \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                                 \
                                 CHUPA_TYPE_DECOMPOSER_DESCRIPTION,     \
                                 ChupaDecomposerDescriptionPrivate))

typedef struct _ChupaDecomposerDescriptionPrivate ChupaDecomposerDescriptionPrivate;
struct _ChupaDecomposerDescriptionPrivate
{
    gchar *name;
    gchar *label;
    GList *mime_types;
    ChupaDecomposerFactory *factory;
};

enum
{
    PROP_0,
    PROP_NAME,
    PROP_LABEL,
    PROP_FACTORY
};

#define chupa_decomposer_description_init init
G_DEFINE_TYPE(ChupaDecomposerDescription,
              chupa_decomposer_description,
              G_TYPE_OBJECT)
#undef chupa_decomposer_description_init

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);
static void
chupa_decomposer_description_class_init (ChupaDecomposerDescriptionClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("name",
                               "The name of the decomposer",
                               "The name of the decomposer",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

    spec = g_param_spec_string("label",
                               "The label of the decomposer",
                               "The label of the decomposer",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LABEL, spec);

    spec = g_param_spec_object("factory",
                               "The factory of the decomposer",
                               "The factory of the decomposer",
                               CHUPA_TYPE_DECOMPOSER_FACTORY,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FACTORY, spec);

    g_type_class_add_private(gobject_class,
                             sizeof(ChupaDecomposerDescriptionPrivate));
}

static void
init (ChupaDecomposerDescription *description)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description);
    priv->name = NULL;
    priv->label = NULL;
    priv->mime_types = NULL;
    priv->factory = NULL;
}

static void
dispose_factory(ChupaDecomposerDescription *description)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description);

    if (priv->factory) {
        g_object_unref(priv->factory);
        priv->factory = NULL;
    }
}

static void
dispose(GObject *object)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(object);

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    if (priv->label) {
        g_free(priv->label);
        priv->label = NULL;
    }

    if (priv->mime_types) {
        g_list_foreach(priv->mime_types, (GFunc)g_free, NULL);
        g_list_free(priv->mime_types);
        priv->mime_types = NULL;
    }

    dispose_factory(CHUPA_DECOMPOSER_DESCRIPTION(object));

    G_OBJECT_CLASS(chupa_decomposer_description_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_NAME:
        if (priv->name)
            g_free(priv->name);
        priv->name = g_value_dup_string(value);
        break;
    case PROP_LABEL:
        if (priv->label)
            g_free(priv->label);
        priv->label = g_value_dup_string(value);
        break;
    case PROP_FACTORY:
        if (priv->factory)
            g_object_unref(priv->factory);
        priv->factory = g_value_get_object(value);
        if (priv->factory)
            g_object_ref(priv->factory);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
    case PROP_LABEL:
        g_value_set_string(value, priv->label);
        break;
    case PROP_FACTORY:
        g_value_set_object(value, priv->factory);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

ChupaDecomposerDescription *
chupa_decomposer_description_new(const gchar *name)
{
    return g_object_new(CHUPA_TYPE_DECOMPOSER_DESCRIPTION,
                        "name", name,
                        NULL);
}

const gchar *
chupa_decomposer_description_get_name (ChupaDecomposerDescription *description)
{
    return CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description)->name;
}

const gchar *
chupa_decomposer_description_get_label (ChupaDecomposerDescription *description)
{
    return CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description)->label;
}

void
chupa_decomposer_description_set_label (ChupaDecomposerDescription *description,
                                        const gchar *label)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description);
    if (priv->label)
        g_free(priv->label);
    priv->label = g_strdup(label);
}

GList *
chupa_decomposer_description_get_mime_types (ChupaDecomposerDescription *description)
{
    return CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description)->mime_types;
}

void
chupa_decomposer_description_add_mime_type (ChupaDecomposerDescription *description,
                                            const gchar *mime_type)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description);
    if (chupa_utils_string_equal(mime_type, "x-chupatext/x-dynamic")) {
        ChupaDecomposerFactory *factory;
        GList *mime_types, *node;

        factory = chupa_decomposer_description_get_factory(description);
        if (!factory)
            return;
        mime_types = chupa_decomposer_factory_get_mime_types(factory);
        for (node = mime_types; node; node = g_list_next(node)) {
            const gchar *dynamic_mime_type = node->data;
            chupa_info("[mime-type][register][dynamic][%s][%s]: <%s>",
                       priv->name, priv->label, dynamic_mime_type);
            priv->mime_types = g_list_append(priv->mime_types,
                                             g_strdup(dynamic_mime_type));
        }
    } else {
        chupa_info("[mime-type][register][%s][%s]: <%s>",
                   priv->name, priv->label, mime_type);
        priv->mime_types = g_list_append(priv->mime_types, g_strdup(mime_type));
    }
}

ChupaDecomposerFactory *
chupa_decomposer_description_get_factory (ChupaDecomposerDescription *description)
{
    ChupaDecomposerDescriptionPrivate *priv;

    priv = CHUPA_DECOMPOSER_DESCRIPTION_GET_PRIVATE(description);
    if (!priv->factory) {
        priv->factory = chupa_decomposer_factory_new(priv->name);
    }
    return priv->factory;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
