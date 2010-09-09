/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_text_input_stream.h"

G_DEFINE_TYPE(ChupaTextInputStream, chupa_text_input_stream, G_TYPE_INPUT_STREAM)

#define CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
                                 CHUPA_TYPE_TEXT_INPUT_STREAM, \
                                 ChupaTextInputStreamPrivate))

typedef struct _ChupaTextInputStreamPrivate ChupaTextInputStreamPrivate;

struct _ChupaTextInputStreamPrivate
{
    ChupaTextInput *input;
};

enum {
    PROP_0,
    PROP_INPUT,
    PROP_DUMMY
};

enum {
    peek_buffer_size = 1024
};

static void
chupa_text_input_stream_init(ChupaTextInputStream *stream)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    priv->input = NULL;
}

static void
dispose(GObject *object)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(object);
    if (priv->input) {
        g_object_unref(priv->input);
        priv->input = NULL;
    }
    G_OBJECT_CLASS(chupa_text_input_stream_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaTextInputStreamPrivate *priv;
    GObject *obj;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        obj = g_value_dup_object(value);
        priv->input = CHUPA_TEXT_INPUT(obj);
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
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        g_value_set_object(value, priv->input);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gssize
read_fn(GInputStream *stream, void *buffer, gsize count,
        GCancellable *cancellable, GError **error)
{
    ChupaTextInputStreamPrivate *priv;
    GsfInput *input;
    gsf_off_t remaining;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    input = chupa_text_input_get_base_input(priv->input);
    remaining = gsf_input_remaining(input);
    if (count > remaining) {
        count = remaining;
    }
    if (!gsf_input_read(input, count, buffer)) {
        return 0;
    }
    return remaining;
}

static gssize
skip_fn(GInputStream *stream, gsize count,
        GCancellable *cancellable, GError **error)
{
    ChupaTextInputStreamPrivate *priv;
    GsfInput *input;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    input = chupa_text_input_get_base_input(priv->input);
    if (gsf_input_seek(input, count, G_SEEK_CUR)) {
        return (gssize)-1;
    }
    return gsf_input_tell(input);
}

static gboolean
close_fn(GInputStream *stream, GCancellable *cancellable, GError **error)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    if (priv->input) {
        g_object_unref(priv->input);
        priv->input = NULL;
    }
}

static void
chupa_text_input_stream_class_init(ChupaTextInputStreamClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GInputStreamClass *input_stream_class = G_INPUT_STREAM_CLASS(klass);
    GParamSpec *spec;

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    input_stream_class->read_fn = read_fn;
    input_stream_class->skip    = skip_fn;
    input_stream_class->close_fn = close_fn;

    spec = g_param_spec_object("input",
                               "Input",
                               "Input",
                               CHUPA_TYPE_TEXT_INPUT,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_INPUT, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaTextInputStreamPrivate));
}

ChupaTextInputStream *
chupa_text_input_stream_new(ChupaTextInput *input)
{
    return g_object_new(CHUPA_TYPE_TEXT_INPUT_STREAM,
                        "input", input,
                        NULL);
}

