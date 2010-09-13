/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/archive_decomposer.h>
#include <chupatext/chupa_module.h>
#include <glib.h>
#include <gsf/gsf-infile-tar.h>

#define CHUPA_TYPE_TAR_DECOMPOSER            chupa_type_tar_decomposer
#define CHUPA_TAR_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTARDecomposer)
#define CHUPA_TAR_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTARDecomposerClass)
#define CHUPA_IS_TAR_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TAR_DECOMPOSER)
#define CHUPA_IS_TAR_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TAR_DECOMPOSER)
#define CHUPA_TAR_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTARDecomposerClass)

typedef struct _ChupaTARDecomposer ChupaTARDecomposer;
typedef struct _ChupaTARDecomposerClass ChupaTARDecomposerClass;

struct _ChupaTARDecomposer
{
    ChupaArchiveDecomposer parent_object;
};

struct _ChupaTARDecomposerClass
{
    ChupaArchiveDecomposerClass parent_class;
};

static GType chupa_type_tar_decomposer = 0;

static void
chupa_tar_decomposer_init(ChupaTARDecomposer *dec)
{
}

static void
chupa_tar_decomposer_class_init(ChupaTARDecomposerClass *klass)
{
    ChupaArchiveDecomposerClass *dec_class = CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass);
    dec_class->get_infile = gsf_infile_tar_new;
}

static GType
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaTARDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_tar_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaTARDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };
    GType type = chupa_type_tar_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_ARCHIVE_DECOMPOSER,
                                           "ChupaTARDecomposer",
                                           &info, 0);
        chupa_type_tar_decomposer = type;
        chupa_decomposer_register("application/x-tar", type);
        chupa_decomposer_register("application/x-compressed-tar", type);
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
    return g_object_new_valist(CHUPA_TYPE_TAR_DECOMPOSER,
                               first_property, var_args);
}
