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
#include "chupa_module_factory.h"
#include "chupa_module_factory_utils.h"

#define CHUPA_MODULE_FACTORY_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                        \
                                 CHUPA_TYPE_MODULE_FACTORY,    \
                                 ChupaModuleFactoryPrivate))

typedef struct _ChupaModuleFactoryPrivate ChupaModuleFactoryPrivate;
struct _ChupaModuleFactoryPrivate
{
    GList *mime_types;
};

#define chupa_module_factory_init init
G_DEFINE_ABSTRACT_TYPE(ChupaModuleFactory, chupa_module_factory, G_TYPE_OBJECT)
#undef chupa_module_factory_init

static void dispose        (GObject         *object);

static void
chupa_module_factory_class_init (ChupaModuleFactoryClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = dispose;

    g_type_class_add_private(gobject_class, sizeof(ChupaModuleFactoryPrivate));
}

static void
init (ChupaModuleFactory *factory)
{
    ChupaModuleFactoryPrivate *priv;

    priv = CHUPA_MODULE_FACTORY_GET_PRIVATE(factory);
    priv->mime_types = NULL;
}

static void
dispose(GObject *object)
{
    ChupaModuleFactoryPrivate *priv;

    priv = CHUPA_MODULE_FACTORY_GET_PRIVATE(object);
    if (priv->mime_types) {
        g_list_foreach(priv->mime_types, (GFunc)g_free, NULL);
        g_list_free(priv->mime_types);
        priv->mime_types = NULL;
    }

    G_OBJECT_CLASS(chupa_module_factory_parent_class)->dispose(object);
}

ChupaModuleFactory *
chupa_module_factory_new_valist (const gchar *type, const gchar *name,
                                 const gchar *first_property, va_list var_args)
{
    ChupaModule *module;
    GObject *factory;

    module = chupa_module_factory_load_module(type, name);
    g_return_val_if_fail(module != NULL, NULL);

    factory = chupa_module_instantiate(module, first_property, var_args);

    return CHUPA_MODULE_FACTORY(factory);
}

ChupaModuleFactory *
chupa_module_factory_new (const gchar *type, const gchar *name,
                        const gchar *first_property, ...)
{
    ChupaModule *module;
    GObject *factory;
    va_list var_args;

    module = chupa_module_factory_load_module(type, name);
    if (!module)
        return NULL;

    va_start(var_args, first_property);
    factory = chupa_module_instantiate(module, first_property, var_args);
    va_end(var_args);

    return CHUPA_MODULE_FACTORY(factory);
}

GObject *
chupa_module_factory_create(ChupaModuleFactory *factory)
{
    ChupaModuleFactoryClass *klass;

    g_return_val_if_fail(CHUPA_IS_MODULE_FACTORY(factory), NULL);

    klass = CHUPA_MODULE_FACTORY_GET_CLASS(factory);
    g_return_val_if_fail(klass->create, NULL);

    return klass->create(factory);
}

GList *
chupa_module_factory_get_mime_types(ChupaModuleFactory *factory)
{
    ChupaModuleFactoryPrivate *priv;

    g_return_val_if_fail(CHUPA_IS_MODULE_FACTORY(factory), NULL);

    priv = CHUPA_MODULE_FACTORY_GET_PRIVATE(factory);
    if (!priv->mime_types) {
        ChupaModuleFactoryClass *klass;
        klass = CHUPA_MODULE_FACTORY_GET_CLASS(factory);
        g_return_val_if_fail(klass->get_mime_types, NULL);
        priv->mime_types = klass->get_mime_types(factory);
    }

    return priv->mime_types;
}


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
