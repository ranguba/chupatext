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

#define CHUPA_DISPATCHER_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                    \
                                 CHUPA_TYPE_DISPATCHER,    \
                                 ChupaDispatcherPrivate))

typedef struct _ChupaDispatcherPrivate    ChupaDispatcherPrivate;
struct _ChupaDispatcherPrivate
{
    gchar *module_dir;
    GList *factories;
    GHashTable *mime_type_table;
};

enum
{
    PROP_0,
    PROP_MODULE_DIR
};

G_DEFINE_TYPE(ChupaDispatcher, chupa_dispatcher, G_TYPE_OBJECT)

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
chupa_dispatcher_class_init (ChupaDispatcherClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("module-dir",
                               "The name of the directory",
                               "The name of the directory in which includes target factory",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MODULE_DIR, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaDispatcherPrivate));
}

#define module_list_free (GDestroyNotify)g_list_free

static gchar *
detect_module_dir(void)
{
    const gchar *dir;

    dir = g_getenv("CHUPA_FACTORY_DIR");
    if (!dir) {
#ifdef G_OS_WIN32
        if (!win32_decomposer_factory_module_dir)
            win32_decomposer_factory_module_dir =
                chupa_win32_build_factory_module_dir_name("decomposer");
        dir = win32_decomposer_factory_module_dir;
#else
        dir = FACTORY_MODULE_DIR;
#endif
    }
    return g_strdup(dir);
}

static void
register_factory(ChupaDispatcherPrivate *priv,
                 const gchar *mime_type, ChupaModuleFactory *factory)
{
    gpointer key = (gpointer)mime_type, value = NULL;

    if (g_hash_table_lookup_extended(priv->mime_type_table, key, &key, &value)) {
        g_hash_table_steal(priv->mime_type_table, key);
    }
    value = g_list_prepend((GList *)value, factory);
    g_hash_table_insert(priv->mime_type_table, key, value);
}

#if 0
static void
unregister_factory(ChupaDispatcherPrivate *priv,
                   const gchar *mime_type, ChupaModuleFactory *factory)
{
    gpointer key = (gpointer)mime_type, value = NULL;

    if (g_hash_table_lookup_extended(priv->mime_type_table, key, &key, &value)) {
        g_hash_table_steal(priv->mime_type_table, key);
        value = g_list_remove((GList *)value, factory);
        if (value) {
            g_hash_table_insert(priv->mime_type_table, key, value);
        }
    }
}
#endif

static void
load_factories (ChupaDispatcherPrivate *priv)
{
    GList *factories = NULL, *node;
    GList *factory_names;

    factory_names = chupa_module_factory_get_names("decomposer");

    for (node = factory_names; node; node = g_list_next(node)) {
        ChupaModuleFactory *module_factory;
        GList *mime_types;

        module_factory = chupa_module_factory_new("decomposer", node->data, NULL);
        factories = g_list_prepend(factories, module_factory);
        mime_types = chupa_module_factory_get_mime_types(module_factory);
        for (; mime_types; mime_types = g_list_next(mime_types)) {
            const gchar *mime_type = mime_types->data;
            register_factory(priv, mime_type, module_factory);
        }
    }

    priv->factories = g_list_reverse(factories);
}

static void
chupa_dispatcher_init (ChupaDispatcher *dispatcher)
{
    ChupaDispatcherPrivate *priv;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(dispatcher);
    priv->module_dir = detect_module_dir();
    priv->mime_type_table = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                  NULL, module_list_free);
    load_factories(priv);
}

static void
dispose(GObject *object)
{
    ChupaDispatcherPrivate *priv;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(object);
    if (priv->module_dir) {
        g_free(priv->module_dir);
        priv->module_dir = NULL;
    }

    if (priv->factories) {
        g_list_foreach(priv->factories, (GFunc)g_object_unref, NULL);
        g_list_free(priv->factories);
        priv->factories = NULL;
    }

    if (priv->mime_type_table) {
        g_hash_table_unref(priv->mime_type_table);
        priv->mime_type_table = NULL;
    }

    G_OBJECT_CLASS(chupa_dispatcher_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    ChupaDispatcherPrivate *priv = CHUPA_DISPATCHER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_MODULE_DIR:
        if (priv->module_dir)
            g_free(priv->module_dir);
        priv->module_dir = g_value_dup_string(value);
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
    ChupaDispatcherPrivate *priv = CHUPA_DISPATCHER_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_MODULE_DIR:
        g_value_set_string(value, priv->module_dir);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

const gchar *
chupa_dispatcher_get_module_dir (ChupaDispatcher *dispatcher)
{
    return CHUPA_DISPATCHER_GET_PRIVATE(dispatcher)->module_dir;
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
    GList *factories = NULL;
    const char *sub_type;
    gpointer key, value;
    int retry = 0;

    priv = CHUPA_DISPATCHER_GET_PRIVATE(dispatcher);
    do {
        if (g_hash_table_lookup_extended(priv->mime_type_table, mime_type,
                                         &key, &value)) {
            factories = (GList *)value;
        }
        else if ((sub_type = strchr((const char *)mime_type, '/')) != NULL) {
            GString *tmp_type = NULL;
            ++sub_type;
            if (sub_type[0] == 'x' && sub_type[1] == '-') {
                tmp_type = g_string_new_len(mime_type, sub_type - mime_type);
                g_string_append(tmp_type, sub_type += 2);
                if (g_hash_table_lookup_extended(priv->mime_type_table,
                                                 tmp_type->str, &key, &value)) {
                    factories = (GList *)value;
                }
            }
            if (tmp_type) {
                g_string_free(tmp_type, TRUE);
            }
        }
    } while (!factories && !retry++);

    if (!factories) {
        return NULL;
    }
    return CHUPA_DECOMPOSER(chupa_module_factory_create(factories->data));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
