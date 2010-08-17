/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext.h>

G_DEFINE_TYPE(ChupaText, chupatext, G_TYPE_OBJECT)

const char chupatext_signal_decomposed[] = "decomposed";

enum {
    DECOMPOSED,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

static void
chupatext_class_init(ChupaTextClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    /*GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS(klass);*/

    g_type_class_add_private(gobject_class, sizeof(ChupaTextPrivate));

    gsignals[DECOMPOSED] =
        g_signal_new(chupatext_signal_ready,
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaTextClass, ready),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,__marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, G_TYPE_INPUT_STREAM);
}
