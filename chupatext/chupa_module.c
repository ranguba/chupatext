/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include "chupatext/config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>

#include <gmodule.h>

#include "chupatext/chupa_module.h"
#include "chupatext/chupa_module_impl.h"
#if !defined G_PLATFORM_WIN32 && defined HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#define CHUPA_MODULE_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_MODULE, \
                                 ChupaModulePrivate))

typedef struct _ChupaModulePrivate  ChupaModulePrivate;
struct _ChupaModulePrivate
{
    GModule      *library;
    gchar        *mod_path;
    GList        *registered_types;

    ChupaModuleInitFunc         init;
    ChupaModuleExitFunc         exit;
    ChupaModuleInstantiateFunc  instantiate;
};

G_DEFINE_TYPE(ChupaModule, chupa_module, G_TYPE_TYPE_MODULE)

static const char chupa_module_prefix[] = "chupa_";

static void     finalize(GObject     *object);
static gboolean load(GTypeModule *module);
static void     unload(GTypeModule *module);

static void     _chupa_module_show_error(GModule     *module);
static GModule *_chupa_module_open(const gchar *mod_path);
static void     _chupa_module_close(GModule     *module);
static gboolean _chupa_module_load_func(GModule     *module,
                                        const gchar *func_name,
                                        gpointer    *symbol);
static gboolean _chupa_module_match_name(const gchar *mod_path,
                                         const gchar *name);

static void
chupa_module_class_init(ChupaModuleClass *klass)
{
    GObjectClass *gobject_class;
    GTypeModuleClass *type_module_class;

    gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize     = finalize;

    type_module_class = G_TYPE_MODULE_CLASS(klass);
    type_module_class->load     = load;
    type_module_class->unload   = unload;

    g_type_class_add_private(gobject_class, sizeof(ChupaModulePrivate));
}

static void
chupa_module_init(ChupaModule *module)
{
    ChupaModulePrivate *priv = CHUPA_MODULE_GET_PRIVATE(module);

    priv->library          = NULL;
    priv->mod_path         = NULL;
    priv->registered_types = NULL;
}

static void
finalize(GObject *object)
{
    ChupaModulePrivate *priv = CHUPA_MODULE_GET_PRIVATE(object);

    g_free(priv->mod_path);
    priv->mod_path = NULL;
    g_list_free(priv->registered_types);
    priv->registered_types = NULL;

    G_OBJECT_CLASS(chupa_module_parent_class)->finalize(object);
}

static gboolean
load(GTypeModule *module)
{
    ChupaModulePrivate *priv = CHUPA_MODULE_GET_PRIVATE(module);

    priv->library = _chupa_module_open(priv->mod_path);
    if (!priv->library)
        return FALSE;

    if (!_chupa_module_load_func(priv->library,
                                 G_STRINGIFY(CHUPA_MODULE_IMPL_INIT),
                                 (gpointer)&priv->init) ||
        !_chupa_module_load_func(priv->library,
                                 G_STRINGIFY(CHUPA_MODULE_IMPL_EXIT),
                                 (gpointer)&priv->exit) ||
        !_chupa_module_load_func(priv->library,
                                 G_STRINGIFY(CHUPA_MODULE_IMPL_INSTANTIATE),
                                 (gpointer)&priv->instantiate)) {
        _chupa_module_close(priv->library);
        priv->library = NULL;
        return FALSE;
    }

    g_list_free(priv->registered_types);
    priv->registered_types = priv->init(module);

    return TRUE;
}

static void
cleanup(ChupaModule *module)
{
    ChupaModulePrivate *priv;

    priv = CHUPA_MODULE_GET_PRIVATE(module);

    if (!priv->exit)
        return;

    priv->exit();

    _chupa_module_close(priv->library);
    priv->library  = NULL;

    priv->init = NULL;
    priv->exit = NULL;
    priv->instantiate = NULL;

    g_list_free(priv->registered_types);
    priv->registered_types = NULL;
}

static void
unload(GTypeModule *module)
{
    cleanup(CHUPA_MODULE(module));
}

GList *
chupa_module_collect_registered_types(GList *modules)
{
    GList *results = NULL;
    GList *node;

    for (node = modules; node; node = g_list_next(node)) {
        ChupaModule *module = node->data;
        GTypeModule *g_type_module;

        g_type_module = G_TYPE_MODULE(module);
        if (g_type_module_use(g_type_module)) {
            ChupaModulePrivate *priv;
            GList *node;

            priv = CHUPA_MODULE_GET_PRIVATE(module);
            for (node = priv->registered_types;
                 node;
                 node = g_list_next(node)) {
                results = g_list_prepend(results, node->data);
            }

            g_type_module_unuse(g_type_module);
        }
    }

    return results;
}

GList *
chupa_module_collect_names(GList *modules)
{
    GList *results = NULL;
    GList *node;

    for (node = modules; node; node = g_list_next(node)) {
        ChupaModule *module;

        module = node->data;
        results = g_list_prepend(results, G_TYPE_MODULE(module)->name);
    }

    return results;
}

static void
_chupa_module_show_error(GModule *module)
{
    const gchar *module_error_message;

    module_error_message = g_module_error();
    if (!module_error_message)
        return;

    if (module) {
        g_warning("%s: %s", g_module_name(module), module_error_message);
    }
    else {
        g_warning("%s", module_error_message);
    }
}

ChupaModule *
chupa_module_find(GList *modules, const gchar *name)
{
    GList *node;

    for (node = modules; node; node = g_list_next(node)) {
        ChupaModule *module = node->data;
        ChupaModulePrivate *priv;

        priv = CHUPA_MODULE_GET_PRIVATE(module);
        if (_chupa_module_match_name(priv->mod_path, name))
            return module;
    }

    return NULL;
}

GObject *
chupa_module_instantiate(ChupaModule *module,
                         const gchar *first_property, va_list var_args)
{
    GObject *object = NULL;
    ChupaModulePrivate *priv;

    priv = CHUPA_MODULE_GET_PRIVATE(module);
    if (g_type_module_use(G_TYPE_MODULE(module))) {
        object = priv->instantiate(first_property, var_args);
        g_type_module_unuse(G_TYPE_MODULE(module));
    }

    return object;
}


static GModule *
_chupa_module_open(const gchar *mod_path)
{
    GModule *module;

    module = g_module_open(mod_path, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
    if (!module) {
        _chupa_module_show_error(NULL);
    }

    return module;
}

static void
_chupa_module_close(GModule *module)
{
    if (module && g_module_close(module)) {
        _chupa_module_show_error(NULL);
    }
}

static gchar *
_chupa_module_module_file_name(const gchar *name)
{
    return g_strconcat(name, "." G_MODULE_SUFFIX, NULL);
}

static gboolean
_chupa_module_load_func(GModule *module, const gchar *func_name,
                       gpointer *symbol)
{
    g_return_val_if_fail(module, FALSE);

    if (g_module_symbol(module, func_name, symbol)) {
        return TRUE;
    } else {
        _chupa_module_show_error(module);
        return FALSE;
    }
}

ChupaModule *
chupa_module_load_module(const gchar *base_dir, const gchar *name)
{
    gchar *mod_base_name, *mod_path;
    ChupaModule *module = NULL;

    mod_base_name = g_build_filename(base_dir, name, NULL);
    if (g_str_has_suffix(mod_base_name, G_MODULE_SUFFIX)) {
        mod_path = mod_base_name;
    } else {
        mod_path = _chupa_module_module_file_name(mod_base_name);
        g_free(mod_base_name);
    }

    if (g_file_test(mod_path, G_FILE_TEST_EXISTS)) {
        ChupaModulePrivate *priv;

        module = chupa_module_new(name, NULL, NULL, NULL);
        priv = CHUPA_MODULE_GET_PRIVATE(module);
        priv->mod_path = mod_path;
    } else {
        g_free(mod_path);
    }

    return module;
}

GList *
chupa_module_load_modules(const gchar *base_dir)
{
    return chupa_module_load_modules_unique(base_dir, NULL);
}

GList *
chupa_module_load_modules_unique(const gchar *base_dir,
                                 GList *exist_modules)
{
    GDir *dir;
    GSList *sorted_entries = NULL;
    GSList *node = NULL;
    GList *modules = NULL;
    const gchar *entry;

    dir = g_dir_open(base_dir, 0, NULL);
    if (!dir)
        return exist_modules;

    while ((entry = g_dir_read_name(dir))) {
        if (!g_str_has_prefix(entry, chupa_module_prefix))
            continue;
        if (!g_str_has_suffix(entry, "."G_MODULE_SUFFIX))
            continue;
        if (chupa_module_find(exist_modules, entry))
            continue;
        sorted_entries = g_slist_prepend(sorted_entries, g_strdup(entry));
    }
    sorted_entries = g_slist_sort(sorted_entries, (GCompareFunc)strcmp);
    for (node = sorted_entries; node; node = g_slist_next(node)) {
        ChupaModule *module;
        GTypeModule *g_module;

        entry = node->data;
        module = chupa_module_load_module(base_dir, entry);
        if (!module)
            continue;

        g_module = G_TYPE_MODULE(module);
        modules = g_list_prepend(modules, module);
    }
    g_slist_foreach(sorted_entries, (GFunc)g_free, NULL);
    g_slist_free(sorted_entries);
    g_dir_close(dir);

    return g_list_concat(modules, exist_modules);
}

static gboolean
_chupa_module_match_name(const gchar *mod_path, const gchar *name)
{
    gboolean matched;
    gchar *module_base_name, *normalized_matched_name;

    module_base_name = g_path_get_basename(mod_path);
    normalized_matched_name = g_strconcat(name, "." G_MODULE_SUFFIX, NULL);

    matched = (0 == strcmp(module_base_name, normalized_matched_name));

    g_free(module_base_name);
    g_free(normalized_matched_name);

    return matched;
}

void
chupa_module_unload(ChupaModule *module)
{
    GTypeModule *type_module;

    g_return_if_fail(CHUPA_IS_MODULE(module));

    type_module = G_TYPE_MODULE(module);

    if (type_module->type_infos || type_module->interface_infos) {
        cleanup(module);
    } else {
        g_object_unref(module);
    }
}

#ifdef G_PLATFORM_WIN32

#ifdef DLL_EXPORT
static HMODULE chupa_dll = NULL;

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
      chupa_dll = hinstDLL;

  return TRUE;
}
#else
#define chupa_dll NULL
#endif

#define get_package_dir() g_win32_get_package_installation_directory_of_module(chupa_dll)

#else
static gchar *
get_package_dir(void)
{
#if defined HAVE_DLFCN_H
    Dl_info dli;
    if (dladdr((gpointer)get_package_dir, &dli)) {
        return g_strdup(dli.dli_fname);
    }
#endif
    return NULL;
}
#endif

gchar *
chupa_module_path(void)
{
    const gchar *path = g_getenv("CHUPA_MODULE_PATH");
    if (!path && !g_path_is_absolute(path = CHUPA_MODULE_PATH)) {
        gchar *base_dir;
#if defined G_OS_WIN32
        base_dir = g_win32_get_package_installation_directory_of_module(chupa_dll);
#elif defined HAVE_DLFCN_H
        base_dir = get_package_dir();
#endif
        if (base_dir) {
            return g_build_filename(base_dir, path, NULL);
        }
    }
    return g_strdup(path);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
