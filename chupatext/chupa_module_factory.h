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

#ifndef __CHUPA_MODULE_FACTORY_H__
#define __CHUPA_MODULE_FACTORY_H__

#include <glib-object.h>

#include <chupatext/chupa_module.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_MODULE_FACTORY            (chupa_module_factory_get_type ())
#define CHUPA_MODULE_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_MODULE_FACTORY, ChupaModuleFactory))
#define CHUPA_MODULE_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_MODULE_FACTORY, ChupaModuleFactoryClass))
#define CHUPA_IS_MODULE_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_MODULE_FACTORY))
#define CHUPA_IS_MODULE_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_MODULE_FACTORY))
#define CHUPA_MODULE_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_MODULE_FACTORY, ChupaModuleFactoryClass))

typedef struct _ChupaModuleFactory         ChupaModuleFactory;
typedef struct _ChupaModuleFactoryClass    ChupaModuleFactoryClass;

struct _ChupaModuleFactory
{
    GObject object;
};

struct _ChupaModuleFactoryClass
{
    GObjectClass parent_class;

    GObject     *(*create)              (ChupaModuleFactory *factory, const gchar *label);
    GList       *(*get_mime_types)      (ChupaModuleFactory *factory);
};

GType        chupa_module_factory_get_type    (void) G_GNUC_CONST;

ChupaModuleFactory *chupa_module_factory_new    (const gchar *type,
                                                 const gchar *name,
                                                 const gchar *first_property,
                                                 ...);
ChupaModuleFactory *chupa_module_factory_new_valist
                                                (const gchar *type,
                                                 const gchar *name,
                                                 const gchar *first_property,
                                                 va_list      var_args);

GObject     *chupa_module_factory_create        (ChupaModuleFactory *factory, const gchar *label);

GList       *chupa_module_factory_get_mime_types(ChupaModuleFactory *factory);

G_END_DECLS

#endif /* __CHUPA_MODULE_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
