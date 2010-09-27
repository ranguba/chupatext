/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#include <glib.h>
#include <string.h>

#include "chupa_decomposer.h"
#include "chupa_module.h"
#include "chupa_module_factory.h"

#define CHUPA_DECOMPOSER_GET_PRIVATE(obj)                       \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj,                           \
                                 CHUPA_TYPE_DECOMPOSER_OBJECT,  \
                                 ChupaDecomposerPrivate))

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
typedef struct _ChupaDecomposerPrivate  ChupaDecomposerPrivate;
struct _ChupaDecomposerPrivate
{
};
#endif

G_DEFINE_ABSTRACT_TYPE(ChupaDecomposer, chupa_decomposer, G_TYPE_OBJECT)

static void
chupa_decomposer_init(ChupaDecomposer *decomposer)
{
}

static void
chupa_decomposer_class_init(ChupaDecomposerClass *klass)
{
#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    g_type_class_add_private(gobject_class, sizeof(ChupaDecomposerPrivate));
#endif
}

static GList *decomposers = NULL;
static gchar *decomposer_module_dir = NULL;
#ifdef G_OS_WIN32
static gchar *win32_decomposer_module_dir = NULL;
#endif

static const gchar *
_chupa_decomposer_module_dir (void)
{
    const gchar *dir;

    if (decomposer_module_dir)
        return decomposer_module_dir;

    dir = g_getenv("CHUPA_DECOMPOSER_DIR");
    if (dir)
        return dir;

#ifdef G_OS_WIN32
    if (!win32_decomposer_module_dir)
        win32_decomposer_module_dir =
            chupa_win32_build_module_dir_name("decomposer");
    return win32_decomopser_module_dir;
#else
    return DECOMPOSER_MODULE_DIR;
#endif
}

static ChupaModule *
_chupa_decomposer_load_module (const gchar *decomposer)
{
    ChupaModule *module;

    module = chupa_module_find(decomposers, decomposer);
    if (module)
        return module;

    module = chupa_module_load_module(_chupa_decomposer_module_dir(),
                                      decomposer);
    if (module) {
        if (g_type_module_use(G_TYPE_MODULE(module))) {
            decomposers = g_list_prepend(decomposers, module);
            g_type_module_unuse(G_TYPE_MODULE(module));
        }
    }

    return module;
}

ChupaDecomposer *
chupa_decomposer_new (const gchar *name, const gchar *first_property, ...)
{
    ChupaModule *module;
    GObject *decomposer;
    va_list var_args;

    module = _chupa_decomposer_load_module(name);
    g_return_val_if_fail(module != NULL, NULL);

    va_start(var_args, first_property);
    decomposer = chupa_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CHUPA_DECOMPOSER(decomposer);
}

gboolean
chupa_decomposer_feed(ChupaDecomposer *dec, ChupaText *text, ChupaTextInput *input, GError **err)
{
    g_return_val_if_fail(CHUPA_IS_DECOMPOSER(dec), FALSE);
    return (*CHUPA_DECOMPOSER_GET_CLASS(dec)->feed)(dec, text, input, err);
}
