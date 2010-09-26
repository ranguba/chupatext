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
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTarDecomposer)
#define CHUPA_TAR_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTarDecomposerClass)
#define CHUPA_IS_TAR_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TAR_DECOMPOSER)
#define CHUPA_IS_TAR_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TAR_DECOMPOSER)
#define CHUPA_TAR_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTarDecomposerClass)

typedef struct _ChupaTarDecomposer ChupaTarDecomposer;
typedef struct _ChupaTarDecomposerClass ChupaTarDecomposerClass;

struct _ChupaTarDecomposer
{
    ChupaArchiveDecomposer parent_object;
};

struct _ChupaTarDecomposerClass
{
    ChupaArchiveDecomposerClass parent_class;
};

static GType chupa_type_tar_decomposer = 0;

static void
chupa_tar_decomposer_init(ChupaTarDecomposer *dec)
{
}

static void
chupa_tar_decomposer_class_init(ChupaTarDecomposerClass *klass)
{
    ChupaArchiveDecomposerClass *dec_class = CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass);
    dec_class->get_infile = gsf_infile_tar_new;
}

static void
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaTarDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_tar_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaTarDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };

    chupa_type_tar_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_ARCHIVE_DECOMPOSER,
                                    "ChupaTarDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    registered_types =
        g_list_prepend(registered_types,
                       (gchar *)g_type_name(chupa_type_tar_decomposer));

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
