/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

enum {
    ACCEPTED,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

static void dispose        (GObject         *object);

static void
chupa_feeder_class_init(ChupaFeederClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(gobject_class, sizeof(ChupaFeederPrivate));

    gobject_class->dispose      = dispose;

    signals[ACCEPTED] =
        g_signal_new("accepted",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaFeederClass, accepted),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, CHUPA_TYPE_DATA);
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
    return g_quark_from_static_string("chupa-feeder-error-quark");
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
 * chupa_feeder_accepted:
 * @feeder: the #ChupaFeeder instance to be signaled.
 * @data: the input to extract from.
 *
 * This function is protected, and should be called from subclass of
 * #ChupaFeederDecomposer only.
 */
void
chupa_feeder_accepted(ChupaFeeder *feeder, ChupaData *data)
{
    g_signal_emit(feeder, signals[ACCEPTED], 0, data);
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
chupa_feeder_feed(ChupaFeeder *feeder, ChupaData *data, GError **error)
{
    ChupaFeederPrivate *priv;
    const char *mime_type = NULL;
    ChupaDecomposer *dec;
    gboolean result;

    g_return_val_if_fail(feeder != NULL, FALSE);
    g_return_val_if_fail(data != NULL, FALSE);

    priv = CHUPA_FEEDER_GET_PRIVATE(feeder);
    mime_type = chupa_data_get_mime_type(data);

    if (!mime_type) {
        g_set_error(error,
                    CHUPA_FEEDER_ERROR,
                    CHUPA_FEEDER_ERROR_UNKNOWN_CONTENT,
                    "can't determin mime-type");
        result = FALSE;
    } else if ((dec = chupa_dispatcher_dispatch(priv->dispatcher, mime_type))) {
        result = chupa_decomposer_feed(dec, feeder, data, error);
        g_object_unref(dec);
    } else {
        g_set_error(error,
                    CHUPA_FEEDER_ERROR,
                    CHUPA_FEEDER_ERROR_UNKNOWN_MIME_TYPE,
                    "unknown mime-type %s", mime_type);
        result = FALSE;
    }
    return result;
}

static void
cb_accepted(ChupaFeeder *feeder, ChupaData *data, gpointer user_data)
{
    ChupaData **text_data = user_data;

    *text_data = g_object_ref(data);
}

/**
 * chupa_feeder_decompose:
 *
 * @feeder: the #ChupaFeeder instance.
 * @data: the input to extract from.
 * @error: return location for an error, or %NULL.
 *
 * Feeds @data to @feeder and returns extracted text data.
 *
 * Returns: a text data as #ChupaData.
 */
ChupaData *
chupa_feeder_decompose(ChupaFeeder *feeder, ChupaData *data, GError **error)
{
    ChupaData *text_data = NULL;
    g_signal_connect(feeder, "accepted", (GCallback)cb_accepted, &text_data);
    chupa_feeder_feed(feeder, data, error);
    return text_data;
}
