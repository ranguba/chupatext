/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_restrict_input_stream.h"

G_DEFINE_TYPE(ChupaRestrictInputStream, chupa_restrict_input_stream, G_TYPE_FILTER_INPUT_STREAM)

#define CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
                                 CHUPA_TYPE_RESTRICT_INPUT_STREAM, \
                                 ChupaRestrictInputStreamPrivate))

#define PARENT_CLASS chupa_restrict_input_stream_parent_class

typedef struct _ChupaRestrictInputStreamPrivate ChupaRestrictInputStreamPrivate;

struct _ChupaRestrictInputStreamPrivate
{
    goffset current, start, length;
};

enum {
    PROP_0,
    PROP_START,
    PROP_LENGTH,
    PROP_DUMMY
};

static void
chupa_restrict_input_stream_init(ChupaRestrictInputStream *stream)
{
    ChupaRestrictInputStreamPrivate *priv;

    priv = CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(stream);
    priv->current = 0;
    priv->start = 0;
    priv->length = 0;
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaRestrictInputStreamPrivate *priv;
    goffset ofs;

    priv = CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_LENGTH:
        ofs = g_value_get_int64(value);
        if (ofs < 0) {
            g_error("%s: negative length %"G_GOFFSET_FORMAT, G_STRLOC, ofs);
        }
        else {
            priv->length = ofs;
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property(GObject *object,
             guint prop_id,
             GValue *value,
             GParamSpec *pspec)
{
    ChupaRestrictInputStreamPrivate *priv;

    priv = CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_LENGTH:
        g_value_set_int64(value, priv->length);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gssize
restrict_read(GInputStream *stream, void *buffer, gsize count,
              GCancellable *cancellable, GError **error)
{
    ChupaRestrictInputStreamPrivate *priv = CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(stream);
    gssize limit = priv->length - priv->current, result;

    if (count > limit) {
        count = limit;
    }
    result = G_INPUT_STREAM_CLASS(PARENT_CLASS)->read_fn(stream, buffer, count, cancellable, error);
    if (result > 0) {
        priv->current += result;
    }

    return result;
}

static gssize
restrict_skip(GInputStream *stream, gsize count,
              GCancellable *cancellable, GError **error)
{
    ChupaRestrictInputStreamPrivate *priv = CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(stream);
    gssize limit = priv->length - priv->current, result;

    if (count > limit) {
        count = limit;
    }
    result = G_INPUT_STREAM_CLASS(PARENT_CLASS)->skip(stream, count, cancellable, error);
    if (result > 0) {
        priv->current += result;
    }
    return result;
}

static void
chupa_restrict_input_stream_class_init(ChupaRestrictInputStreamClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GInputStreamClass *input_class = G_INPUT_STREAM_CLASS(klass);
    GParamSpec *spec;

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    input_class->read_fn = restrict_read;
    input_class->skip = restrict_skip;

    spec = g_param_spec_int64("length",
                              "Length",
                              "Length of the input stream",
                              0, G_MAXINT64, 0,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_LENGTH, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaRestrictInputStreamPrivate));
}

GInputStream *
chupa_restrict_input_stream_new(GInputStream *input, goffset length)
{
    return g_object_new(CHUPA_TYPE_RESTRICT_INPUT_STREAM,
                        "base-stream", input,
                        "length", length,
                        NULL);
}

gsize
chupa_restrict_input_stream_skip_to_end(GInputStream *stream)
{
    ChupaRestrictInputStreamPrivate *priv = CHUPA_RESTRICT_INPUT_STREAM_GET_PRIVATE(stream);
    gssize rest = priv->length - priv->current, result;

    result = G_INPUT_STREAM_CLASS(PARENT_CLASS)->skip(stream, rest, NULL, NULL);
    if (result > 0) {
        priv->current += result;
    }
    return result;
}
