/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_text_input_stream.h"

G_DEFINE_TYPE(ChupaTextInputStream, chupa_text_input_stream, G_TYPE_DATA_INPUT_STREAM)

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

enum {
    peek_buffer_size = 1024
};

static const char *
guess_mime_type(GInputStream *stream, gboolean *uncertain)
{
    const char *mime_type;

    if (G_IS_FILE_INPUT_STREAM(stream)) {
        GFileInfo *info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(stream),
                                                         G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                                                         NULL, NULL);
        mime_type = g_content_type_get_mime_type(g_file_info_get_content_type(info));
        g_object_unref(info);
    }
    else {
        gsize len;
        const char *buf;
        GBufferedInputStream *buffered = G_BUFFERED_INPUT_STREAM(stream);

        g_buffered_input_stream_fill(buffered, peek_buffer_size, NULL, NULL);
        buf = g_buffered_input_stream_peek_buffer(buffered, &len);
        mime_type = g_content_type_get_mime_type(g_content_type_guess(NULL, buf, len, uncertain));
    }
    return mime_type;
}

static void
chupa_text_input_stream_init(ChupaTextInputStream *stream)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    priv->metadata = NULL;
}

static void
constructed(GObject *object)
{
    ChupaTextInputStream *stream = CHUPA_TEXT_INPUT_STREAM(object);
    ChupaTextInputStreamPrivate *priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    const gchar *mime_type;

    if (!priv->metadata) {
        priv->metadata = chupa_metadata_new();
    }

    mime_type = guess_mime_type(G_INPUT_STREAM(stream), NULL);
    chupa_metadata_replace_value(priv->metadata, "mime-type", mime_type);
}

static void
dispose(GObject *object)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(object);
    if (priv->metadata) {
        g_object_unref(priv->metadata);
        priv->metadata = NULL;
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
    case PROP_METADATA:
        obj = g_value_dup_object(value);
        priv->metadata = CHUPA_METADATA(obj);
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
    case PROP_METADATA:
        g_value_set_object(value, priv->metadata);
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
    GParamSpec *spec;

    gobject_class->constructed  = constructed;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("metadata",
                               "Metadata",
                               "Metadata of the input stream",
                               CHUPA_TYPE_METADATA,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_METADATA, spec);
    g_type_class_add_private(gobject_class, sizeof(ChupaTextInputStreamPrivate));
}

ChupaTextInputStream *
chupa_text_input_stream_new(ChupaMetadata *metadata, GInputStream *input)
{
    if (CHUPA_IS_TEXT_INPUT_STREAM(input)) {
        ChupaTextInputStream *ti = (ChupaTextInputStream *)input;
        if (metadata) {
            chupa_metadata_merge(chupa_text_input_stream_get_metadata(ti), metadata);
        }
        return ti;
    }
    return g_object_new(CHUPA_TYPE_TEXT_INPUT_STREAM,
                        "metadata", metadata,
                        "base-stream", input,
                        NULL);
}

ChupaMetadata *
chupa_text_input_stream_get_metadata(ChupaTextInputStream *stream)
{
    ChupaTextInputStreamPrivate *priv;

    priv = CHUPA_TEXT_INPUT_STREAM_GET_PRIVATE(stream);
    return priv->metadata;
}
