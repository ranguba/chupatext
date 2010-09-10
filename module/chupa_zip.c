/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/archive_decomposer.h>
#include <chupatext/chupa_module.h>
#include <glib.h>
#include <gsf/gsf-infile-zip.h>

#define CHUPA_TYPE_ZIP_DECOMPOSER            chupa_type_zip_decomposer
#define CHUPA_ZIP_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_ZIP_DECOMPOSER, ChupaZIPDecomposer)
#define CHUPA_ZIP_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_ZIP_DECOMPOSER, ChupaZIPDecomposerClass)
#define CHUPA_IS_ZIP_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_ZIP_DECOMPOSER)
#define CHUPA_IS_ZIP_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_ZIP_DECOMPOSER)
#define CHUPA_ZIP_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_ZIP_DECOMPOSER, ChupaZIPDecomposerClass)

typedef struct _ChupaZIPDecomposer ChupaZIPDecomposer;
typedef struct _ChupaZIPDecomposerClass ChupaZIPDecomposerClass;

struct _ChupaZIPDecomposer
{
    ChupaArchiveDecomposer parent_object;
};

struct _ChupaZIPDecomposerClass
{
    ChupaArchiveDecomposerClass parent_class;
};

static GType chupa_type_zip_decomposer = 0;

static void
chupa_zip_decomposer_init(ChupaZIPDecomposer *dec)
{
}

static void
chupa_zip_decomposer_class_init(ChupaZIPDecomposerClass *klass)
{
}

static GType
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaZIPDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_zip_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaZIPDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };
    GType type = chupa_type_zip_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_ARCHIVE_DECOMPOSER,
                                           "ChupaZIPDecomposer",
                                           &info, 0);
        chupa_type_zip_decomposer = type;
        chupa_decomposer_register("application/zip", type);
    }
    return type;
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GType type = register_type(type_module);
    GList *registered_types = NULL;

#if 0
    if (type) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(type));
    }
#endif

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_ZIP_DECOMPOSER,
                               first_property, var_args);
}
