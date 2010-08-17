/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_MODULE_H__
#define CHUPATEXT_MODULE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_MODULE            chupa_module_get_type()
#define CHUPA_MODULE(obj)            G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_MODULE, ChupaModule)
#define CHUPA_MODULE_CLASS(klass)    G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_MODULE, ChupaModuleClass)
#define CHUPA_IS_MODULE(obj)         G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_MODULE)
#define CHUPA_IS_MODULE_CLASS(klass) G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_MODULE)
#define CHUPA_MODULE_GET_CLASS(obj)  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_MODULE, ChupaModuleClass)

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

GType        chupa_module_get_type(void) G_GNUC_CONST;

ChupaModule *chupa_module_load_module(const gchar *base_dir,
                                      const gchar *name);
GList       *chupa_module_load_modules(const gchar *base_dir);
GList       *chupa_module_load_modules_unique(const gchar *base_dir,
                                              GList *modules);
ChupaModule *chupa_module_find(GList *modules,
                               const gchar *name);

GObject     *chupa_module_instantiate(ChupaModule *module,
                                      const gchar *first_property,
                                      va_list var_args);
GList       *chupa_module_collect_registered_types(GList *modules);
GList       *chupa_module_collect_names(GList *modules);
void         chupa_module_unload(ChupaModule *module);

G_END_DECLS

#endif /* CHUPATEXT_MODULE_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
