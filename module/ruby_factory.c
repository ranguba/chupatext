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

#ifdef HAVE_CONFIG_H
#  include <chupatext/config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>

#include <chupatext/chupa_module_impl.h>
#include <chupatext/chupa_module_factory.h>
#include <chupatext/chupa_decomposer.h>

#define CHUPA_TYPE_RUBY_FACTORY            chupa_type_ruby_factory
#define CHUPA_RUBY_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_RUBY_FACTORY, ChupaRUBYFactory))
#define CHUPA_RUBY_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_RUBY_FACTORY, ChupaRUBYFactoryClass))
#define CHUPA_IS_RUBY_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_RUBY_FACTORY))
#define CHUPA_IS_RUBY_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_RUBY_FACTORY))
#define CHUPA_RUBY_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_RUBY_FACTORY, ChupaRUBYFactoryClass))

typedef struct _ChupaRUBYFactory ChupaRUBYFactory;
typedef struct _ChupaRUBYFactoryClass ChupaRUBYFactoryClass;

struct _ChupaRUBYFactory
{
    ChupaModuleFactory     object;
};

struct _ChupaRUBYFactoryClass
{
    ChupaModuleFactoryClass parent_class;
};

static GType chupa_type_ruby_factory = 0;
static ChupaModuleFactoryClass *parent_class;

static GList     *get_mime_types   (ChupaModuleFactory    *factory);
static GObject   *create           (ChupaModuleFactory    *factory);

static void
class_init (ChupaModuleFactoryClass *klass)
{
    GObjectClass *gobject_class;
    ChupaModuleFactoryClass *factory_class;

    parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class  = CHUPA_MODULE_FACTORY_CLASS(klass);

    factory_class->get_mime_types   = get_mime_types;
    factory_class->create           = create;
}

static void
init (ChupaRUBYFactory *factory)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (ChupaRUBYFactoryClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaRUBYFactory),
            0,
            (GInstanceInitFunc) init,
        };

    chupa_type_ruby_factory =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_MODULE_FACTORY,
                                    "ChupaRUBYFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (chupa_type_ruby_factory) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(chupa_type_ruby_factory));
    }

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_RUBY_FACTORY, first_property, var_args);
}

static GList *
get_mime_types (ChupaModuleFactory *factory)
{
    GList *mime_types = NULL;

    mime_types = g_list_prepend(mime_types, g_strdup("application/ruby"));

    return mime_types;
}

GObject *
create (ChupaModuleFactory *factory)
{
    static int n_instances = 0;
    GObject *decomposer;

    decomposer = G_OBJECT(chupa_decomposer_new("ruby", NULL));
    n_instances++;
    if (n_instances == 1) {
        /* FIXME: This causes memory leak but we need to
         * keep at least 1 decomposer instance to avoid
         * duplicated non-module GType
         * (e.g. PopplerDocument) registration. */
        g_object_ref(decomposer);
    }
    return decomposer;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
