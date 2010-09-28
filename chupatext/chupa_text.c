/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include "chupatext/chupa_text.h"
#include "chupatext/chupa_decomposer.h"
#include "chupatext/chupa_dispatcher.h"
#include "chupatext/chupa_module_factory_utils.h"

#define CHUPA_TEXT_GET_PRIVATE(obj)                        \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                    \
                                 CHUPA_TYPE_TEXT,          \
                                 ChupaTextPrivate))

G_DEFINE_TYPE(ChupaText, chupa_text, G_TYPE_OBJECT)

typedef struct ChupaTextPrivate {
    ChupaDispatcher *dispatcher;
} ChupaTextPrivate;

const char chupa_text_signal_decomposed[] = "decomposed";

enum {
    DECOMPOSED,
    LAST_SIGNAL
};

static gint gsignals[LAST_SIGNAL] = {0};

static void dispose        (GObject         *object);

static void
chupa_text_class_init(ChupaTextClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(gobject_class, sizeof(ChupaTextPrivate));

    gobject_class->dispose      = dispose;

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
    ChupaTextPrivate *priv;

    priv = CHUPA_TEXT_GET_PRIVATE(chupar);

    priv->dispatcher = chupa_dispatcher_new();
}

static void
dispose (GObject *object)
{
    ChupaTextPrivate *priv = CHUPA_TEXT_GET_PRIVATE(object);

    if (priv->dispatcher) {
        g_object_unref(priv->dispatcher);
        priv->dispatcher = NULL;
    }

    G_OBJECT_CLASS(chupa_text_parent_class)->dispose(object);
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
    ChupaTextPrivate *priv;
    const char *mime_type = NULL;
    ChupaDecomposer *dec;
    GError *e;
    gboolean result;

    g_return_val_if_fail(chupar != NULL, FALSE);
    g_return_val_if_fail(input != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    priv = CHUPA_TEXT_GET_PRIVATE(chupar);
    mime_type = chupa_text_input_get_mime_type(input);

    if (!mime_type) {
        e = chupa_text_error_new_literal(CHUPA_TEXT_ERROR_UNKNOWN_CONTENT,
                                         "can't determin mime-type");
        g_propagate_error(error, e);
        result = FALSE;
    }
    else if ((dec = chupa_dispatcher_dispatch(priv->dispatcher, mime_type))) {
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
    g_signal_connect(chupar, chupa_text_signal_decomposed, (GCallback)func, arg);
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
