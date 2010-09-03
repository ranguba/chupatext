/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_restrict_input_stream.h>
#include <glib.h>
#include <wv.h>

#define CHUPA_TYPE_MSWORD_DECOMPOSER chupa_type_msword_decomposer
#define CHUPA_MSWORD_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_MSWORD_DECOMPOSER, ChupaMSWORDDecomposer)
#define CHUPA_MSWORD_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_MSWORD_DECOMPOSER, ChupaMSWORDDecomposerClass)
#define CHUPA_IS_MSWORD_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_MSWORD_DECOMPOSER)
#define CHUPA_IS_MSWORD_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_MSWORD_DECOMPOSER)
#define CHUPA_MSWORD_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_MSWORD_DECOMPOSER, ChupaMSWORDDecomposerClass)

typedef struct _ChupaMSWORDDecomposer ChupaMSWORDDecomposer;
typedef struct _ChupaMSWORDDecomposerClass ChupaMSWORDDecomposerClass;

struct _ChupaMSWORDDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaMSWORDDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_msword_decomposer = 0;

static void
chupa_msword_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInputStream *stream)
{
    GInputStream *input = G_INPUT_STREAM(stream);
}

static void
chupa_msword_decomposer_class_init(ChupaMSWORDDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_msword_decomposer_feed;
}

static GType
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaMSWORDDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_msword_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaMSWORDDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };
    GType type = chupa_type_msword_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_DECOMPOSER,
                                           "ChupaMSWORDDecomposer",
                                           &info, 0);
        chupa_type_msword_decomposer = type;
        chupa_decomposer_register("application/x-msword", type);
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
    return g_object_new_valist(CHUPA_TYPE_MSWORD_DECOMPOSER,
                               first_property, var_args);
}
