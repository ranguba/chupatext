/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_text_input_stream.h"

G_DEFINE_TYPE(ChupaTextInputStream, chupa_text_input_stream, G_TYPE_OBJECT)

#define CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
                                 CHUPA_TYPE_TEXT_INPUT_STREAM, \
                                 ChupaTextInputStreamPrivate))

typedef struct _ChupaTextInputStreamPrivate ChupaTextInputStreamPrivate;

struct _ChupaTextInputStreamPrivate
{
    ChupaMetadata *metadata;
};

enum {
    PROP_0,
    PROP_METADATA,
    PROP_DUMMY
};

static void
chupa_text_input_stream_init(ChupaTextInputStream *stream)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    priv->metadata = NULL;
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_METADATA:
        priv->metadata = CHUPA_METADATA(g_value_dup_object(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
chupa_text_input_stream_class_init(ChupaTextInputStreamClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    g_type_class_add_private(gobject_class, sizeof(ChupaTextInputStreamPrivate));
}

ChupaTextInputStream *
chupa_text_input_stream_new(ChupaMetadata *metadata, GInputStream *input)
{
}

ChupaMetadata *
chupa_text_input_stream_get_metadata(ChupaTextInputStream *stream)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    return priv->metadata;
}
