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
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>

#include "chupa_dispatcher.h"
#include "chupa_decomposer_factory.h"
#include "chupa_decomposer_description.h"
#include "chupa_logger.h"
#include "chupa_utils.h"

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
chupa_dispatcher_init (ChupaDispatcher *dispatcher)
{
    ChupaDispatcherPrivate *priv;
    GList *descriptions, *node;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(dispatcher);
    priv->descriptions = NULL;
    descriptions = chupa_decomposer_description_get_list();
    for (node = descriptions; node; node = g_list_next(node)) {
        ChupaDecomposerDescription *description = node->data;
        priv->descriptions = g_list_prepend(priv->descriptions,
                                            g_object_ref(description));
    }
    priv->descriptions = g_list_reverse(priv->descriptions);
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
    ChupaDecomposerFactory *factory = NULL;
    GObject *decomposer = NULL;
    GString *normalized_type = NULL;
    const gchar *sub_type;
    const gchar *label = NULL;
    const gchar *real_mime_type = NULL;

    sub_type = strchr(mime_type, '/');
    if (sub_type) {
        sub_type++;
        if (g_str_has_prefix(sub_type, "x-")) {
            normalized_type = g_string_new_len(mime_type,
                                               sub_type - mime_type);
            g_string_append(normalized_type, sub_type + 2);
        }
    }

    priv = CHUPA_DISPATCHER_GET_PRIVATE(dispatcher);
    for (node = priv->descriptions; node; node = g_list_next(node)) {
        ChupaDecomposerDescription *description = node->data;
        GList *mime_types;

        mime_types = chupa_decomposer_description_get_mime_types(description);
        for (; mime_types; mime_types = g_list_next(mime_types)) {
            if ((chupa_utils_string_equal(mime_type, mime_types->data))) {
                factory = chupa_decomposer_description_get_factory(description);
                real_mime_type = mime_type;
                break;
            } else if (normalized_type &&
                       chupa_utils_string_equal(normalized_type->str,
                                                mime_types->data)) {
                factory = chupa_decomposer_description_get_factory(description);
                real_mime_type = normalized_type->str;
                break;
            }
        }
        if (factory) {
            label = chupa_decomposer_description_get_label(description);
            break;
        }
    }

    if (factory) {
        GError *error = NULL;
        chupa_info("[dispatcher][%s][factory][found][%s]: <%s>:<%s>",
                   mime_type, label,
                   G_OBJECT_CLASS_NAME(G_OBJECT_GET_CLASS(factory)),
                   real_mime_type);
        decomposer = chupa_decomposer_factory_create(factory, label,
                                                     real_mime_type, &error);
        if (decomposer) {
            chupa_info("[dispatcher][%s][decomposer][create][%s]: <%s>(%s)",
                       mime_type, label,
                       G_OBJECT_CLASS_NAME(G_OBJECT_GET_CLASS(decomposer)),
                       real_mime_type);
        } else {
            chupa_log_g_error(error,
                              "[dispatcher][%s][decomposer][create][error][%s]"
                              ": (%s)",
                              mime_type, label, real_mime_type);
            g_error_free(error);
        }
    }

    if (normalized_type)
        g_string_free(normalized_type, TRUE);

    if (!decomposer)
        return NULL;

    return CHUPA_DECOMPOSER(decomposer);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
