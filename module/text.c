/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module_impl.h>
#include <glib.h>

#define CHUPA_TYPE_TEXT_DECOMPOSER chupa_type_text_decomposer
#define CHUPA_TEXT_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT_DECOMPOSER, ChupaTextDecomposer)
#define CHUPA_TEXT_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT_DECOMPOSER, ChupaTextDecomposerClass)
#define CHUPA_IS_TEXT_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT_DECOMPOSER)
#define CHUPA_IS_TEXT_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT_DECOMPOSER)
#define CHUPA_TEXT_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT_DECOMPOSER, ChupaTextDecomposerClass)

typedef struct _ChupaTextDecomposer ChupaTextDecomposer;
typedef struct _ChupaTextDecomposerClass ChupaTextDecomposerClass;

struct _ChupaTextDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaTextDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_text_decomposer = 0;

static gboolean
chupa_text_decomposer_feed(ChupaDecomposer *decomposer, ChupaText *chupar,
                           ChupaTextInput *input, GError **err)
{
    chupa_text_decomposed(chupar, input);
    return TRUE;
}

static void
chupa_text_decomposer_class_init(ChupaTextDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_text_decomposer_feed;
}

static void
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaTextDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_text_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaTextDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };

    chupa_type_text_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    "ChupaTextDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (chupa_type_text_decomposer) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(chupa_type_text_decomposer));
    }

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_TEXT_DECOMPOSER,
                               first_property, var_args);
}
