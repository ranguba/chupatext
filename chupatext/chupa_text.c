/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_text.h"
#include "chupatext/chupa_decomposer.h"

G_DEFINE_TYPE(ChupaText, chupa_text, G_TYPE_OBJECT)

#ifdef USE_CHUPA_TEXT_PRIVATE
typedef struct ChupaTextPrivate {
    
} ChupaTextPrivate;
#endif

const char chupa_text_signal_decomposed[] = "decomposed";

enum {
    DECOMPOSED,
    LAST_SIGNAL
};

static gint gsignals[LAST_SIGNAL] = {0};

static void
chupa_text_class_init(ChupaTextClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    /*GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS(klass);*/

#ifdef USE_CHUPA_TEXT_PRIVATE
    g_type_class_add_private(gobject_class, sizeof(ChupaTextPrivate));
#endif

    gsignals[DECOMPOSED] =
        g_signal_new(chupa_text_signal_decomposed,
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaTextClass, decomposed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, G_TYPE_INPUT_STREAM);
}

static void
chupa_text_init(ChupaText *chupar)
{
#ifdef USE_CHUPA_TEXT_PRIVATE
    ChupaTextPrivate *priv;

    priv = CHUPA_TEXT_GET_PRIVATE(chupar);
#endif
}

ChupaText *
chupa_text_new(void)
{
    ChupaText *chupar;
    chupar = g_object_new(CHUPA_TYPE_TEXT,
                          NULL);
    return chupar;
}

void
chupa_text_decomposed(ChupaText *chupar, ChupaTextInputStream *stream)
{
    g_signal_emit_by_name(chupar, chupa_text_signal_decomposed, stream);
}

void
chupa_text_feed(ChupaText *chupar, GInputStream *stream)
{
    const char *mime_type = NULL;
    ChupaTextInputStream *ti;
    ChupaDecomposer *dec;

    ti = chupa_text_input_stream_new(NULL, stream);
    mime_type = chupa_text_input_stream_get_mime_type(ti);

    if (!mime_type) {
        g_error("can't determin mime-type\n");
    }
    else if (dec = chupa_decomposer_search(mime_type)) {
        chupa_decomposer_feed(dec, chupar, ti);
        g_object_unref(dec);
    }
    else {
        g_warning("unknown mime-type %s\n", mime_type);
    }
    g_object_unref(G_OBJECT(ti));
}
