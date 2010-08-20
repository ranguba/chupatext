/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_decomposer.h"
#include <glib.h>

#define CHUPA_DECOMPOSER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_DECOMPOSER_OBJECT, \
                                 ChupaDecomposerPrivate))

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
typedef struct _ChupaDecomposerPrivate  ChupaDecomposerPrivate;
struct _ChupaDecomposerPrivate
{
};
#endif

G_DEFINE_TYPE(ChupaDecomposer, chupa_decomposer, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_SOURCE,
    PROP_DUMMY
};

static void
chupa_decomposer_init(ChupaDecomposer *decomposer)
{
}

static void
chupa_decomposer_class_init(ChupaDecomposerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    /*GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS(klass);*/

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
    g_type_class_add_private(gobject_class, sizeof(ChupaDecomposerPrivate));
#endif
}

ChupaDecomposer *
chupa_decomposer_new(void)
{
    return g_object_new(CHUPA_TYPE_DECOMPOSER,
                        NULL);
}
