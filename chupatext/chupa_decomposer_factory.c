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
#include <glib/gi18n-lib.h>

#include "chupa_module.h"
#include "chupa_decomposer_factory.h"
#include "chupa_utils.h"

static gchar *module_dir = NULL;
static GList *modules = NULL;
#ifdef G_OS_WIN32
static gchar *win32_module_dir = NULL;
#endif
static GList *factories = NULL;

void
chupa_decomposer_factory_init (void)
{
    modules = NULL;
    factories = NULL;
}

void
chupa_decomposer_factory_quit (void)
{
    g_list_foreach(factories, (GFunc)g_object_unref, NULL);
    g_list_free(factories);
    chupa_decomposer_factory_unload();
    chupa_decomposer_factory_set_default_module_dir(NULL);
}

const gchar *
chupa_decomposer_factory_get_default_module_dir (void)
{
    return module_dir;
}

void
chupa_decomposer_factory_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

const gchar *
chupa_decomposer_factory_get_module_dir (void)
{
    const gchar *dir;

    if (module_dir)
        return module_dir;

    dir = g_getenv("CHUPA_DECOMPOSER_DIR");
    if (dir)
        return dir;

#ifdef G_OS_WIN32
    if (!win32_module_dir)
        win32_module_dir = chupa_win32_build_factory_module_dir_name(NULL);
    return win32_module_dir;
#else
    return CHUPA_DECOMPOSER_MODULE_DIR;
#endif
}

void
chupa_decomposer_factory_load (const gchar *dir)
{
    if (g_file_test(dir, G_FILE_TEST_IS_DIR)) {
        modules = g_list_concat(modules, chupa_module_load_modules(dir));
    }
}

void
chupa_decomposer_factory_load_all (const gchar *base_dir)
{
    GDir *gdir;
    const gchar *entry;

    if (!base_dir)
        base_dir = chupa_decomposer_factory_get_module_dir();

    gdir = g_dir_open(base_dir, 0, NULL);
    if (!gdir)
        return;

    while ((entry = g_dir_read_name(gdir))) {
        gchar *dir_name;

        dir_name = g_build_filename(base_dir, entry, NULL);
        if (g_file_test(dir_name, G_FILE_TEST_IS_DIR)) {
            GList *modules;

            modules = chupa_module_load_modules(dir_name);
            if (!modules) {
                gchar *libs_dir;

                libs_dir = g_build_filename(dir_name, ".libs", NULL);
                if (g_file_test(libs_dir, G_FILE_TEST_IS_DIR))
                    modules = chupa_module_load_modules(libs_dir);
                g_free(libs_dir);
            }
            modules = g_list_concat(modules, modules);
        }
        g_free(dir_name);
    }
    g_dir_close(gdir);
}

ChupaModule *
chupa_decomposer_factory_load_module (const gchar *name)
{
    ChupaModule *module;

    module = chupa_module_find(modules, name);
    if (module)
        return module;

    module = chupa_module_load_module(chupa_decomposer_factory_get_module_dir(),
                                      name);
    if (module) {
        modules = g_list_prepend(modules, module);
    }

    return module;
}

void
chupa_decomposer_factory_unload (void)
{
    if (!modules)
        return;

    g_list_foreach(modules, (GFunc)chupa_module_unload, NULL);
    g_list_free(modules);
    modules = NULL;
}

GList *
chupa_decomposer_factory_get_names (void)
{
    if (!modules)
        return NULL;

    return chupa_module_collect_names(modules);
}

gboolean
chupa_decomposer_factory_exist_module (const gchar *name)
{
    GList *names, *list;

    names = chupa_decomposer_factory_get_names();
    if (!names)
        return FALSE;
    list = g_list_find_custom(names, name, (GCompareFunc)strcmp);

    g_list_foreach(names, (GFunc)g_free, NULL);
    g_list_free(names);

    return (list != NULL);
}

#define CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                            \
                                 CHUPA_TYPE_DECOMPOSER_FACTORY,    \
                                 ChupaDecomposerFactoryPrivate))

typedef struct _ChupaDecomposerFactoryPrivate ChupaDecomposerFactoryPrivate;
struct _ChupaDecomposerFactoryPrivate
{
    gchar *name;
    GList *mime_types;
};

enum
{
    PROP_0,
    PROP_NAME
};

#define chupa_decomposer_factory_init init
G_DEFINE_ABSTRACT_TYPE(ChupaDecomposerFactory,
                       chupa_decomposer_factory,
                       G_TYPE_OBJECT)
#undef chupa_decomposer_factory_init

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
chupa_decomposer_factory_class_init (ChupaDecomposerFactoryClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("name",
                               "The name of the module",
                               "The name of the module",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

    g_type_class_add_private(gobject_class,
                             sizeof(ChupaDecomposerFactoryPrivate));
}

static void
init (ChupaDecomposerFactory *factory)
{
    ChupaDecomposerFactoryPrivate *priv;

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(factory);
    priv->name = NULL;
    priv->mime_types = NULL;
}

static void
dispose(GObject *object)
{
    ChupaDecomposerFactoryPrivate *priv;

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(object);

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    if (priv->mime_types) {
        g_list_foreach(priv->mime_types, (GFunc)g_free, NULL);
        g_list_free(priv->mime_types);
        priv->mime_types = NULL;
    }

    G_OBJECT_CLASS(chupa_decomposer_factory_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    ChupaDecomposerFactoryPrivate *priv;

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_NAME:
        if (priv->name)
            g_free(priv->name);
        priv->name = g_value_dup_string(value);
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
    ChupaDecomposerFactoryPrivate *priv;

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

ChupaDecomposerFactory *
chupa_decomposer_factory_new(const gchar *name)
{
    ChupaModule *module;
    GObject *new_factory;
    GList *node;

    for (node = factories; node; node = g_list_next(node)) {
        ChupaDecomposerFactory *factory = node->data;
        ChupaDecomposerFactoryPrivate *priv;

        priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(factory);
        if (chupa_utils_string_equal(priv->name, name)) {
            g_object_ref(factory);
            return factory;
        }
    }

    module = chupa_decomposer_factory_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    new_factory = chupa_module_instantiate(module, name);
    g_object_ref(new_factory);
    factories = g_list_prepend(factories, new_factory);

    return CHUPA_DECOMPOSER_FACTORY(new_factory);
}

GObject *
chupa_decomposer_factory_create(ChupaDecomposerFactory *factory, const gchar *label)
{
    ChupaDecomposerFactoryClass *klass;

    g_return_val_if_fail(CHUPA_IS_DECOMPOSER_FACTORY(factory), NULL);

    klass = CHUPA_DECOMPOSER_FACTORY_GET_CLASS(factory);
    g_return_val_if_fail(klass->create, NULL);

    return klass->create(factory, label);
}

GList *
chupa_decomposer_factory_get_mime_types(ChupaDecomposerFactory *factory)
{
    ChupaDecomposerFactoryPrivate *priv;

    g_return_val_if_fail(CHUPA_IS_DECOMPOSER_FACTORY(factory), NULL);

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(factory);
    if (!priv->mime_types) {
        ChupaDecomposerFactoryClass *klass;
        klass = CHUPA_DECOMPOSER_FACTORY_GET_CLASS(factory);
        g_return_val_if_fail(klass->get_mime_types, NULL);
        priv->mime_types = klass->get_mime_types(factory);
    }

    return priv->mime_types;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
