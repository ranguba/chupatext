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
#include <glib/gi18n-lib.h>

#include "chupa_module.h"
#include "chupa_decomposer_factory.h"

static gchar *module_dir = NULL;
static GList *modules = NULL;
#ifdef G_OS_WIN32
static gchar *win32_factory_module_dir = NULL;
#endif

void
chupa_decomposer_factory_init (void)
{
    modules = NULL;
}

void
chupa_decomposer_factory_quit (void)
{
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

    dir = g_getenv("CHUPA_FACTORY_DIR");
    if (dir)
        return dir;

#ifdef G_OS_WIN32
    if (!win32_factory_module_dir)
        win32_factory_module_dir =
            chupa_win32_build_factory_module_dir_name(NULL);
    return win32_factory_module_dir;
#else
    return FACTORY_MODULE_DIR;
#endif
}

void
chupa_decomposer_factory_load (const gchar *dir, const gchar *type)
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
chupa_decomposer_factory_load_module (const gchar *type, const gchar *name)
{
    ChupaModule *module;

    module = chupa_module_find(modules, name);
    if (module)
        return module;

    module = chupa_module_load_module(chupa_decomposer_factory_get_module_dir(),
                                      name);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            modules = g_list_prepend(modules, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
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
chupa_decomposer_factory_get_names (const gchar *type)
{
    GList *orig_names, *node, *names = NULL;

    if (!modules)
        return NULL;

    orig_names = chupa_module_collect_names(modules);
    for (node = orig_names; node; node = g_list_next(node)) {
        const gchar *name = node->data;

        if (g_str_has_suffix(name, "_factory") ||
            g_str_has_suffix(name, "-factory")) {
            gchar *p;
            p = g_strrstr(name, "factory") - 1;
            names = g_list_prepend(names, g_strndup(name, p - name));
        }
    }
    g_list_free(orig_names);
    return g_list_reverse(names);
}

gboolean
chupa_decomposer_factory_exist_module (const gchar *type, const gchar *name)
{
    GList *names, *list;

    names = chupa_decomposer_factory_get_names(type);
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
    GList *mime_types;
};

#define chupa_decomposer_factory_init init
G_DEFINE_ABSTRACT_TYPE(ChupaDecomposerFactory,
                       chupa_decomposer_factory,
                       G_TYPE_OBJECT)
#undef chupa_decomposer_factory_init

static void dispose        (GObject         *object);

static void
chupa_decomposer_factory_class_init (ChupaDecomposerFactoryClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;

    g_type_class_add_private(gobject_class, sizeof(ChupaDecomposerFactoryPrivate));
}

static void
init (ChupaDecomposerFactory *factory)
{
    ChupaDecomposerFactoryPrivate *priv;

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(factory);
    priv->mime_types = NULL;
}

static void
dispose(GObject *object)
{
    ChupaDecomposerFactoryPrivate *priv;

    priv = CHUPA_DECOMPOSER_FACTORY_GET_PRIVATE(object);
    if (priv->mime_types) {
        g_list_foreach(priv->mime_types, (GFunc)g_free, NULL);
        g_list_free(priv->mime_types);
        priv->mime_types = NULL;
    }

    G_OBJECT_CLASS(chupa_decomposer_factory_parent_class)->dispose(object);
}

ChupaDecomposerFactory *
chupa_decomposer_factory_new_valist (const gchar *type, const gchar *name,
                                 const gchar *first_property, va_list var_args)
{
    ChupaModule *module;
    GObject *factory;

    module = chupa_decomposer_factory_load_module(type, name);
    g_return_val_if_fail(module != NULL, NULL);

    factory = chupa_module_create_factory(module, first_property, var_args);

    return CHUPA_DECOMPOSER_FACTORY(factory);
}

ChupaDecomposerFactory *
chupa_decomposer_factory_new (const gchar *type, const gchar *name,
                          const gchar *first_property, ...)
{
    ChupaModule *module;
    GObject *factory;
    va_list var_args;

    module = chupa_decomposer_factory_load_module(type, name);
    if (!module)
        return NULL;

    va_start(var_args, first_property);
    factory = chupa_module_create_factory(module, first_property, var_args);
    va_end(var_args);

    return CHUPA_DECOMPOSER_FACTORY(factory);
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
