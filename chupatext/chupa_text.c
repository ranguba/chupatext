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
                     G_TYPE_NONE, 1, CHUPA_TYPE_TEXT_INPUT);
}

static void
chupa_text_init(ChupaText *chupar)
{
#ifdef USE_CHUPA_TEXT_PRIVATE
    ChupaTextPrivate *priv;

    priv = CHUPA_TEXT_GET_PRIVATE(chupar);
#endif
}

/**
 * chupa_text_new:
 *
 * Creates a new instance of a #ChupaText type.
 *
 * Returns: a new instance of #ChupaText
 */
ChupaText *
chupa_text_new(void)
{
    ChupaText *chupar;
    chupar = g_object_new(CHUPA_TYPE_TEXT,
                          NULL);
    return chupar;
}

/**
 * chupa_text_decomposed:
 * @chupar: the #ChupaText instance to be signaled.
 * @input: the input to extract from.
 *
 * This function is protected, and should be called from subclass of
 * #ChupaTextDecomposer only.
 */
void
chupa_text_decomposed(ChupaText *chupar, ChupaTextInput *input)
{
    g_signal_emit_by_name(chupar, chupa_text_signal_decomposed, input);
}

/**
 * chupa_text_connect_decomposed:
 *
 * @chupar: the #ChupaText instance to be connected.
 * @func: the callback function to be called with extracted text
 * input.
 * @arg: arbitrary user data.
 *
 * Connect @func to @chupar, so that @func will be called when any
 * text portion is found.
 */
guint
chupa_text_connect_decomposed(ChupaText *chupar, ChupaTextCallback func, gpointer arg)
{
    return g_signal_connect(chupar, chupa_text_signal_decomposed, (GCallback)func, arg);
}

/**
 * chupa_text_feed:
 *
 * @chupar: the #ChupaText instance.
 * @input: the input to extract from.
 *
 * Feeds @input to @chupar, to extract text portions.
 */
gboolean
chupa_text_feed(ChupaText *chupar, ChupaTextInput *input, GError **error)
{
    const char *mime_type = NULL;
    ChupaDecomposer *dec;
    GError *e;
    gboolean result;

    g_return_val_if_fail(chupar != NULL, FALSE);
    g_return_val_if_fail(input != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    mime_type = chupa_text_input_get_mime_type(input);

    if (!mime_type) {
        e = chupa_text_error_new_literal(CHUPA_TEXT_ERROR_UNKNOWN_CONTENT,
                                         "can't determin mime-type");
        g_propagate_error(error, e);
        result = FALSE;
    }
    else if (dec = chupa_decomposer_search(mime_type)) {
        result = chupa_decomposer_feed(dec, chupar, input, error);
        g_object_unref(dec);
        result = TRUE;
    }
    else {
        e = chupa_text_error_new(CHUPA_TEXT_ERROR_UNKNOWN_MIMETYPE,
                                 "unknown mime-type %s", mime_type);
        g_propagate_error(error, e);
        result = FALSE;
    }
    return result;
}

/**
 * chupa_text_decompose:
 *
 * @chupar: the #ChupaText instance.
 * @input: the input to extract from.
 * @func: the callback function to be called with extracted text
 * input.
 * @arg: arbitrary user data.
 *
 * Feeds @input to @chupar, with @func
 */
void
chupa_text_decompose(ChupaText *chupar, ChupaTextInput *input,
                     ChupaTextCallback func, gpointer arg, GError **error)
{
    chupa_text_connect_decomposed(chupar, func, arg);
    chupa_text_feed(chupar, input, error);
}

struct decompose_arg {
    char *read_data;
    gsize length;
    GError **error;
};

static void
text_decomposed(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
{
    GDataInputStream *data = G_DATA_INPUT_STREAM(chupa_text_input_get_stream(input));
    struct decompose_arg *arg = udata;

    arg->read_data = g_data_input_stream_read_until(data, "", &arg->length, NULL, arg->error);
}

/**
 * chupa_text_decompose:
 *
 * @chupar: the #ChupaText instance.
 * @input: the input to extract from.
 *
 * Extracts all text portions from @input.
 *
 * Returns: pointer to the text data that all text portion in @input
 * are combined.
 */
char *
chupa_text_decompose_all(ChupaText *chupar, ChupaTextInput *input, GError **error)
{
    struct decompose_arg arg;
    arg.read_data = NULL;
    arg.length = 0;
    arg.error = error;
    chupa_text_decompose(chupar, input, text_decomposed, &arg, error);
    return arg.read_data;
}
