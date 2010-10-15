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

#include "chupa_feeder.h"
#include "chupa_decomposer.h"
#include "chupa_dispatcher.h"

#define CHUPA_FEEDER_GET_PRIVATE(obj)                   \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                 \
                                 CHUPA_TYPE_FEEDER,     \
                                 ChupaFeederPrivate))

G_DEFINE_TYPE(ChupaFeeder, chupa_feeder, G_TYPE_OBJECT)

typedef struct ChupaFeederPrivate {
    ChupaDispatcher *dispatcher;
} ChupaFeederPrivate;

const char chupa_feeder_signal_decomposed[] = "decomposed";

enum {
    DECOMPOSED,
    LAST_SIGNAL
};

static gint gsignals[LAST_SIGNAL] = {0};

static void dispose        (GObject         *object);

static void
chupa_feeder_class_init(ChupaFeederClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(gobject_class, sizeof(ChupaFeederPrivate));

    gobject_class->dispose      = dispose;

    gsignals[DECOMPOSED] =
        g_signal_new(chupa_feeder_signal_decomposed,
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaFeederClass, decomposed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, CHUPA_TYPE_TEXT_INPUT);
}

static void
chupa_feeder_init(ChupaFeeder *feeder)
{
    ChupaFeederPrivate *priv;

    priv = CHUPA_FEEDER_GET_PRIVATE(feeder);

    priv->dispatcher = chupa_dispatcher_new();
}

static void
dispose (GObject *object)
{
    ChupaFeederPrivate *priv = CHUPA_FEEDER_GET_PRIVATE(object);

    if (priv->dispatcher) {
        g_object_unref(priv->dispatcher);
        priv->dispatcher = NULL;
    }

    G_OBJECT_CLASS(chupa_feeder_parent_class)->dispose(object);
}

GQuark
chupa_feeder_error_quark(void)
{
    static GQuark error_quark = 0;

    if (G_UNLIKELY(error_quark == 0)) {
        error_quark = g_quark_from_static_string("chupa-feeder-error-quark");
    }

    return error_quark;
}

GError *
chupa_feeder_error_new_valist(ChupaFeederError code, const char *format, va_list args)
{
    return g_error_new_valist(CHUPA_FEEDER_ERROR, code, format, args);
}

GError *
chupa_feeder_error_new(ChupaFeederError code, const char *format, ...)
{
    va_list args;
    GError *error;

    va_start(args, format);
    error = chupa_feeder_error_new_valist(code, format, args);
    va_end(args);
    return error;
}

GError *
chupa_feeder_error_new_literal(ChupaFeederError code, const char *message)
{
    return g_error_new_literal(CHUPA_FEEDER_ERROR, code, message);
}

/**
 * chupa_feeder_new:
 *
 * Creates a new instance of a #ChupaFeeder type.
 *
 * Returns: a new instance of #ChupaFeeder
 */
ChupaFeeder *
chupa_feeder_new(void)
{
    ChupaFeeder *feeder;
    feeder = g_object_new(CHUPA_TYPE_FEEDER,
                          NULL);
    return feeder;
}

/**
 * chupa_feeder_decomposed:
 * @feeder: the #ChupaFeeder instance to be signaled.
 * @input: the input to extract from.
 *
 * This function is protected, and should be called from subclass of
 * #ChupaFeederDecomposer only.
 */
void
chupa_feeder_decomposed(ChupaFeeder *feeder, ChupaTextInput *input)
{
    g_signal_emit_by_name(feeder, chupa_feeder_signal_decomposed, input);
}

/**
 * chupa_feeder_feed:
 *
 * @feeder: the #ChupaFeeder instance.
 * @input: the input to extract from.
 *
 * Feeds @input to @feeder, to extract feeder portions.
 */
gboolean
chupa_feeder_feed(ChupaFeeder *feeder, ChupaTextInput *input, GError **error)
{
    ChupaFeederPrivate *priv;
    const char *mime_type = NULL;
    ChupaDecomposer *dec;
    GError *e;
    gboolean result;

    g_return_val_if_fail(feeder != NULL, FALSE);
    g_return_val_if_fail(input != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    priv = CHUPA_FEEDER_GET_PRIVATE(feeder);
    mime_type = chupa_text_input_get_mime_type(input);

    if (!mime_type) {
        e = chupa_feeder_error_new_literal(CHUPA_FEEDER_ERROR_UNKNOWN_CONTENT,
                                         "can't determin mime-type");
        g_propagate_error(error, e);
        result = FALSE;
    }
    else if ((dec = chupa_dispatcher_dispatch(priv->dispatcher, mime_type))) {
        result = chupa_decomposer_feed(dec, feeder, input, error);
        g_object_unref(dec);
        result = TRUE;
    }
    else {
        e = chupa_feeder_error_new(CHUPA_FEEDER_ERROR_UNKNOWN_MIMETYPE,
                                 "unknown mime-type %s", mime_type);
        g_propagate_error(error, e);
        result = FALSE;
    }
    return result;
}

/**
 * chupa_feeder_decompose:
 *
 * @feeder: the #ChupaFeeder instance.
 * @input: the input to extract from.
 * @func: the callback function to be called with extracted feeder
 * input.
 * @arg: arbitrary user data.
 *
 * Feeds @input to @feeder, with @func
 */
void
chupa_feeder_decompose(ChupaFeeder *feeder, ChupaTextInput *input,
                       ChupaFeederCallback func, gpointer arg, GError **error)
{
    g_signal_connect(feeder, chupa_feeder_signal_decomposed, (GCallback)func, arg);
    chupa_feeder_feed(feeder, input, error);
}

struct decompose_arg {
    char *read_data;
    gsize length;
    GError **error;
};

static void
feeder_decomposed(ChupaFeeder *feeder, ChupaTextInput *input, gpointer udata)
{
    GDataInputStream *data = G_DATA_INPUT_STREAM(chupa_text_input_get_stream(input));
    struct decompose_arg *arg = udata;

    arg->read_data = g_data_input_stream_read_until(data, "", &arg->length, NULL, arg->error);
}

/**
 * chupa_feeder_decompose:
 *
 * @feeder: the #ChupaFeeder instance.
 * @input: the input to extract from.
 *
 * Extracts all feeder portions from @input.
 *
 * Returns: pointer to the feeder data that all feeder portion in @input
 * are combined.
 */
char *
chupa_feeder_decompose_all(ChupaFeeder *feeder, ChupaTextInput *input, GError **error)
{
    struct decompose_arg arg;
    arg.read_data = NULL;
    arg.length = 0;
    arg.error = error;
    chupa_feeder_decompose(feeder, input, feeder_decomposed, &arg, error);
    return arg.read_data;
}
