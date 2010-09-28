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

#include "chupatext/chupa_text_input.h"
#include "chupatext/chupa_text_input_stream.h"
#include <gio/gio.h>
#include <gsf/gsf-input-gio.h>

static const char meta_filename[] = "filename";
static const char meta_charset[] = "charset";

/* ChupaTextInput */
G_DEFINE_TYPE(ChupaTextInput, chupa_text_input, G_TYPE_OBJECT)

#define CHUPA_TEXT_INPUT_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
                                 CHUPA_TYPE_TEXT_INPUT, \
                                 ChupaTextInputPrivate))

typedef struct _ChupaTextInputPrivate ChupaTextInputPrivate;

struct _ChupaTextInputPrivate
{
    GsfInput *input;
    ChupaMetadata *metadata;
    GDataInputStream *stream;
};

enum {
    PROP_0,
    PROP_INPUT,
    PROP_STREAM,
    PROP_METADATA,
    PROP_DUMMY
};

const char chupa_text_signal_finished[] = "finished";

enum {
    FINISHED,
    LAST_SIGNAL
};

static gint gsignals[LAST_SIGNAL] = {0};

enum {
    peek_buffer_size = 1024
};

static const char *
guess_mime_type(const char *name, GBufferedInputStream *buffered, gboolean *uncertain)
{
    const char *content_type;
    gsize len;
    const guchar *buf;

    g_buffered_input_stream_fill(buffered, peek_buffer_size, NULL, NULL);
    buf = g_buffered_input_stream_peek_buffer(buffered, &len);
    content_type = g_content_type_guess(name, buf, len, uncertain);
    return g_content_type_get_mime_type(content_type);
}

static void
chupa_text_input_init(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    priv->stream = NULL;
    priv->metadata = NULL;
}

static void
constructed(GObject *object)
{
    ChupaTextInput *input = CHUPA_TEXT_INPUT(object);
    ChupaTextInputPrivate *priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    const gchar *mime_type;
    GInputStream *stream = (GInputStream *)priv->stream;
    const char *path = NULL;

    g_return_if_fail(stream || priv->input);

    if (!priv->metadata) {
        priv->metadata = chupa_metadata_new();
        if (priv->input) {
            path = gsf_input_name(priv->input);
        }
    }
    else {
        path = chupa_metadata_get_first_value(priv->metadata, meta_filename);
    }
    if (!stream) {
        stream = G_INPUT_STREAM(chupa_text_input_stream_new(input));
    }
    if (G_IS_DATA_INPUT_STREAM(stream)) {
        priv->stream = G_DATA_INPUT_STREAM(stream);
    }
    else {
        priv->stream = g_data_input_stream_new(stream);
    }

    mime_type = guess_mime_type(path,
                                G_BUFFERED_INPUT_STREAM(priv->stream),
                                NULL);
    chupa_metadata_replace_value(priv->metadata, "mime-type", mime_type);
}

static void
dispose(GObject *object)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(object);
    if (priv->metadata) {
        g_object_unref(priv->metadata);
        priv->metadata = NULL;
    }

    G_OBJECT_CLASS(chupa_text_input_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaTextInputPrivate *priv;
    GObject *obj;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        obj = g_value_dup_object(value);
        priv->input = GSF_INPUT(obj);
        break;
    case PROP_STREAM:
        obj = g_value_dup_object(value);
        if (obj) {
            if (G_IS_DATA_INPUT_STREAM(obj)) {
                priv->stream = G_DATA_INPUT_STREAM(obj);
            }
            else {
                priv->stream = g_data_input_stream_new(G_INPUT_STREAM(obj));
            }
        }
        break;
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
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        g_value_set_object(value, priv->input);
        break;
    case PROP_STREAM:
        g_value_set_object(value, priv->stream);
        break;
    case PROP_METADATA:
        g_value_set_object(value, priv->metadata);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
chupa_text_input_class_init(ChupaTextInputClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *spec;

    gobject_class->constructed  = constructed;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("input",
                               "Input",
                               "Input",
                               GSF_INPUT_TYPE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_INPUT, spec);

    spec = g_param_spec_object("stream",
                               "Stream",
                               "Stream",
                               G_TYPE_INPUT_STREAM,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_STREAM, spec);

    spec = g_param_spec_object("metadata",
                               "Metadata",
                               "Metadata of the input",
                               CHUPA_TYPE_METADATA,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_METADATA, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaTextInputPrivate));

    gsignals[FINISHED] =
        g_signal_new(chupa_text_signal_finished,
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaTextInputClass, finished),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);
}

ChupaTextInput *
chupa_text_input_new(ChupaMetadata *metadata, GsfInput *input)
{
    const char *path;

    g_return_val_if_fail(input, NULL);
    if ((path = gsf_input_name(input)) != NULL) {
        if (!metadata) {
            metadata = chupa_metadata_new();
        }
        chupa_metadata_add_value(metadata, meta_filename, path);
    }
    return g_object_new(CHUPA_TYPE_TEXT_INPUT,
                        "input", input,
                        "metadata", metadata,
                        NULL);
}

ChupaTextInput *
chupa_text_input_new_from_stream(ChupaMetadata *metadata, GInputStream *stream, const char *path)
{
    g_return_val_if_fail(stream, NULL);
    if (path) {
        if (!metadata) {
            metadata = chupa_metadata_new();
        }
        chupa_metadata_add_value(metadata, meta_filename, path);
    }
    return g_object_new(CHUPA_TYPE_TEXT_INPUT,
                        "stream", stream,
                        "metadata", metadata,
                        NULL);
}

ChupaTextInput *
chupa_text_input_new_from_file(ChupaMetadata *metadata, GFile *file, GError **err)
{
    ChupaTextInput *text;
    GsfInput *input = gsf_input_gio_new(file, err);
    if (!input) {
        return NULL;
    }
    text = chupa_text_input_new(metadata, input);
    g_object_unref(input);
    return text;
}

GsfInput *
chupa_text_input_get_base_input(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    return priv->input;
}

GInputStream *
chupa_text_input_get_stream(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    return G_INPUT_STREAM(priv->stream);
}

ChupaMetadata *
chupa_text_input_get_metadata(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    return priv->metadata;
}

const gchar *
chupa_text_input_get_mime_type(ChupaTextInput *input)
{
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    return chupa_metadata_get_first_value(meta, "mime-type");
}

const gchar *
chupa_text_input_get_filename(ChupaTextInput *input)
{
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    return chupa_metadata_get_first_value(meta, meta_filename);
}

void
chupa_text_input_set_filename(ChupaTextInput *input, const char *filename)
{
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    chupa_metadata_add_value(meta, meta_filename, filename);
}

const gchar *
chupa_text_input_get_charset(ChupaTextInput *input)
{
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    return chupa_metadata_get_first_value(meta, meta_charset);
}

void
chupa_text_input_set_charset(ChupaTextInput *input, const char *charset)
{
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    chupa_metadata_add_value(meta, meta_charset, charset);
}

void
chupa_text_input_finished(ChupaTextInput *input)
{
    g_signal_emit_by_name(input, chupa_text_signal_finished);
}

guint
chupa_text_input_connect_finished(ChupaTextInput *input, ChupaTextInputCallback func, gpointer arg)
{
    return g_signal_connect(input, chupa_text_signal_finished, (GCallback)func, arg);
}
