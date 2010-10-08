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

#define CHUPA_TYPE_TAR_FACTORY            chupa_type_tar_factory
#define CHUPA_TAR_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_TAR_FACTORY, ChupaTarFactory))
#define CHUPA_TAR_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_TAR_FACTORY, ChupaTarFactoryClass))
#define CHUPA_IS_TAR_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_TAR_FACTORY))
#define CHUPA_IS_TAR_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_TAR_FACTORY))
#define CHUPA_TAR_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_TAR_FACTORY, ChupaTarFactoryClass))

typedef struct _ChupaTarFactory ChupaTarFactory;
typedef struct _ChupaTarFactoryClass ChupaTarFactoryClass;

struct _ChupaTarFactory
{
    ChupaModuleFactory     object;
};

struct _ChupaTarFactoryClass
{
    ChupaModuleFactoryClass parent_class;
};

static GType chupa_type_tar_factory = 0;
static ChupaModuleFactoryClass *parent_class;

static GList     *get_mime_types   (ChupaModuleFactory    *factory);
static GObject   *create           (ChupaModuleFactory    *factory, const gchar *label);

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
init (ChupaTarFactory *factory)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (ChupaTarFactoryClass),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaTarFactory),
            0,
            (GInstanceInitFunc) init,
        };

    chupa_type_tar_factory =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_MODULE_FACTORY,
                                    "ChupaTarFactory",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (chupa_type_tar_factory) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(chupa_type_tar_factory));
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
    return g_object_new_valist(CHUPA_TYPE_TAR_FACTORY, first_property, var_args);
}

static GList *
get_mime_types (ChupaModuleFactory *factory)
{
    GList *mime_types = NULL;

    mime_types = g_list_prepend(mime_types, g_strdup("application/tar"));
    mime_types = g_list_prepend(mime_types, g_strdup("application/compressed-tar"));

    return mime_types;
}

static GObject *
create (ChupaModuleFactory *factory, const gchar *label)
{
    return G_OBJECT(chupa_decomposer_new("tar", NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
