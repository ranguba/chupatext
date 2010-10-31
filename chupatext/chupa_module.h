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

#ifndef CHUPA_MODULE_H
#define CHUPA_MODULE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_MODULE            chupa_module_get_type()
#define CHUPA_MODULE(obj)            G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_MODULE, ChupaModule)
#define CHUPA_MODULE_CLASS(klass)    G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_MODULE, ChupaModuleClass)
#define CHUPA_IS_MODULE(obj)         G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_MODULE)
#define CHUPA_IS_MODULE_CLASS(klass) G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_MODULE)
#define CHUPA_MODULE_GET_CLASS(obj)  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_MODULE, ChupaModuleClass)

typedef GList   *(*ChupaModuleInitFunc)        (GTypeModule *module,
                                                GError     **error);
typedef gboolean (*ChupaModuleQuitFunc)        (void);
typedef GObject *(*ChupaModuleInstantiateFunc) (const gchar *first_property,
                                                va_list      va_args);

typedef struct _ChupaModule ChupaModule;
typedef struct _ChupaModuleClass ChupaModuleClass;

struct _ChupaModule
{
    GTypeModule object;
};

struct _ChupaModuleClass
{
    GTypeModuleClass parent_class;
};

GType        chupa_module_get_type       (void) G_GNUC_CONST;

ChupaModule *chupa_module_load_module    (const gchar *base_dir,
                                          const gchar *name);
GList       *chupa_module_load_modules   (const gchar *base_dir);
GList       *chupa_module_load_modules_unique
                                         (const gchar *base_dir,
                                          GList       *modules);
ChupaModule *chupa_module_find           (GList       *modules,
                                          const gchar *name);

GList       *chupa_module_collect_registered_types
                                         (GList *modules);
GList       *chupa_module_collect_names  (GList *modules);
void         chupa_module_unload         (ChupaModule *module);
gchar       *chupa_module_dir            (void);


ChupaModule *chupa_module_new            (const gchar *name,
                                          ChupaModuleInitFunc init,
                                          ChupaModuleQuitFunc quit,
                                          ChupaModuleInstantiateFunc instantiate);

GObject     *chupa_module_instantiate    (ChupaModule *module,
                                          const gchar *name);

G_END_DECLS

#endif /* CHUPA_MODULE_H */

/*
vi:nowrap:ai:expandtab:sw=4
*/
