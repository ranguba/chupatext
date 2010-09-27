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
/* #include <glib/gi18n-lib.h> */

#include "chupa_utils.h"
#include "chupa_module.h"
#include "chupa_module_factory_utils.h"

static gchar *module_dir = NULL;
static GHashTable *factories = NULL;
#ifdef G_OS_WIN32
static gchar *win32_factory_module_dir = NULL;
#endif


static void
unload_modules (gpointer data)
{
    GList *modules = (GList *)data;
    g_list_foreach(modules, (GFunc)chupa_module_unload, NULL);
    g_list_free(modules);
}

void
chupa_module_factory_init (void)
{
    factories = g_hash_table_new_full(g_str_hash,
                                      g_str_equal,
                                      g_free,
                                      unload_modules);
}

void
chupa_module_factory_quit (void)
{
    chupa_module_factory_unload();
    chupa_module_factory_set_default_module_dir(NULL);
}

const gchar *
chupa_module_factory_get_default_module_dir (void)
{
    return module_dir;
}

void
chupa_module_factory_set_default_module_dir (const gchar *dir)
{
    if (module_dir)
        g_free(module_dir);
    module_dir = NULL;

    if (dir)
        module_dir = g_strdup(dir);
}

const gchar *
chupa_module_factory_get_module_dir (void)
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
chupa_module_factory_load (const gchar *dir, const gchar *type)
{
    if (g_file_test(dir, G_FILE_TEST_IS_DIR)) {
        GList *modules;

        modules = chupa_module_load_modules(dir);

        if (modules)
            g_hash_table_replace(factories, g_strdup(type), modules);
    }
}

void
chupa_module_factory_load_all (const gchar *base_dir)
{
    GDir *gdir;
    const gchar *entry;

    if (!base_dir)
        base_dir = chupa_module_factory_get_module_dir();

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
            if (modules)
                g_hash_table_replace(factories, g_strdup(entry), modules);
        }
        g_free(dir_name);
    }
    g_dir_close(gdir);
}

ChupaModule *
chupa_module_factory_load_module (const gchar *type, const gchar *name)
{
    ChupaModule *module;
    GList *modules = NULL;
    gchar *real_name;
    gchar *separators[] = {"_", "-", NULL};
    gchar **separators_p;

    modules = g_hash_table_lookup(factories, type);
    if (!modules)
        return NULL;

    for (separators_p = separators; *separators_p; separators_p++) {
        real_name = g_strconcat(name, *separators_p, "factory", NULL);
        module = chupa_module_find(modules, real_name);
        if (module) {
            g_free(real_name);
            return module;
        }

        module = chupa_module_load_module(chupa_module_factory_get_module_dir(),
                                          real_name);
        g_free(real_name);
        if (module) {
            if (g_type_module_use(G_TYPE_MODULE(module))) {
                modules = g_list_prepend(modules, module);
                g_type_module_unuse(G_TYPE_MODULE(module));
                g_hash_table_replace(factories, g_strdup(type), modules);
            }
            return module;
        }
    }

    return NULL;
}

void
chupa_module_factory_unload (void)
{
    if (!factories)
        return;

    g_hash_table_unref(factories);
    factories = NULL;
}

GList *
chupa_module_factory_get_names (const gchar *type)
{
    GList *modules, *orig_names, *node, *names = NULL;

    modules = g_hash_table_lookup(factories, type);
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
chupa_module_factory_exist_module (const gchar *type, const gchar *name)
{
    GList *names, *list;

    names = chupa_module_factory_get_names(type);
    if (!names)
        return FALSE;
    list= g_list_find_custom(names, name, (GCompareFunc)strcmp);

    g_list_foreach(names, (GFunc)g_free, NULL);
    g_list_free(names);

    return (list != NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
