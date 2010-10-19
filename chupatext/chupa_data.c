/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *  Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#include "chupa_data.h"
#include "chupa_gsf_input_stream.h"
#include <gio/gio.h>
#include <gsf/gsf-input-gio.h>

static const char meta_filename[] = "filename";
static const char meta_charset[] = "charset";

/* ChupaData */
G_DEFINE_TYPE(ChupaData, chupa_data, G_TYPE_OBJECT)

#define CHUPA_DATA_GET_PRIVATE(obj)                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                 \
                                 CHUPA_TYPE_DATA,       \
                                 ChupaDataPrivate))

typedef struct _ChupaDataPrivate ChupaDataPrivate;

struct _ChupaDataPrivate
{
    GsfInput *input;
    ChupaMetadata *metadata;
    GDataInputStream *stream;
    gboolean finished;
    GError *error;
    GString *raw_data;
};

enum {
    PROP_0,
    PROP_INPUT,
    PROP_STREAM,
    PROP_METADATA,
    PROP_DUMMY
};

enum {
    FINISHED,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

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
chupa_data_init(ChupaData *data)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    priv->stream = NULL;
    priv->metadata = NULL;
    priv->finished = FALSE;
    priv->error = NULL;
}

static void
constructed(GObject *object)
{
    ChupaDataPrivate *priv;
    const gchar *mime_type;
    GInputStream *stream;
    const char *path = NULL;

    priv = CHUPA_DATA_GET_PRIVATE(object);
    stream = G_INPUT_STREAM(priv->stream);

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
        stream = G_INPUT_STREAM(chupa_gsf_input_stream_new(priv->input));
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
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(object);

    if (priv->metadata) {
        g_object_unref(priv->metadata);
        priv->metadata = NULL;
    }

    if (priv->error) {
        g_error_free(priv->error);
        priv->error = NULL;
    }

    G_OBJECT_CLASS(chupa_data_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaDataPrivate *priv;
    GObject *obj;

    priv = CHUPA_DATA_GET_PRIVATE(object);
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
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(object);
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
finished(ChupaData *data, GError *error)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);

    if (error) {
        priv->error = g_error_copy(error);
    } else {
        priv->error = NULL;
    }
    priv->finished = TRUE;
}

static void
chupa_data_class_init(ChupaDataClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *spec;

    gobject_class->constructed  = constructed;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    klass->finished = finished;

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

    g_type_class_add_private(gobject_class, sizeof(ChupaDataPrivate));

    signals[FINISHED] =
        g_signal_new("finished",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaDataClass, finished),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOOLEAN,
                     G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

ChupaData *
chupa_data_new(ChupaMetadata *metadata, GsfInput *input)
{
    const char *path;

    g_return_val_if_fail(input, NULL);
    if ((path = gsf_input_name(input)) != NULL) {
        if (!metadata) {
            metadata = chupa_metadata_new();
        }
        chupa_metadata_add_value(metadata, meta_filename, path);
    }
    return g_object_new(CHUPA_TYPE_DATA,
                        "input", input,
                        "metadata", metadata,
                        NULL);
}

ChupaData *
chupa_data_new_from_stream(ChupaMetadata *metadata, GInputStream *stream, const char *path)
{
    g_return_val_if_fail(stream, NULL);
    if (path) {
        if (!metadata) {
            metadata = chupa_metadata_new();
        }
        chupa_metadata_add_value(metadata, meta_filename, path);
    }
    return g_object_new(CHUPA_TYPE_DATA,
                        "stream", stream,
                        "metadata", metadata,
                        NULL);
}

ChupaData *
chupa_data_new_from_file(ChupaMetadata *metadata, GFile *file, GError **err)
{
    ChupaData *data;
    GsfInput *input = gsf_input_gio_new(file, err);
    if (!input) {
        return NULL;
    }
    data = chupa_data_new(metadata, input);
    g_object_unref(input);
    return data;
}

GsfInput *
chupa_data_get_input(ChupaData *data)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    return priv->input;
}

GInputStream *
chupa_data_get_stream(ChupaData *data)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    return G_INPUT_STREAM(priv->stream);
}

ChupaMetadata *
chupa_data_get_metadata(ChupaData *data)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    return priv->metadata;
}

const gchar *
chupa_data_get_mime_type(ChupaData *data)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    return chupa_metadata_get_first_value(meta, "mime-type");
}

void
chupa_data_set_mime_type(ChupaData *data, const gchar *mime_type)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    chupa_metadata_replace_value(meta, "mime-type", mime_type);
}

const gchar *
chupa_data_get_filename(ChupaData *data)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    return chupa_metadata_get_first_value(meta, meta_filename);
}

void
chupa_data_set_filename(ChupaData *data, const char *filename)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    chupa_metadata_add_value(meta, meta_filename, filename);
}

const gchar *
chupa_data_get_charset(ChupaData *data)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    return chupa_metadata_get_first_value(meta, meta_charset);
}

void
chupa_data_set_charset(ChupaData *data, const char *charset)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    chupa_metadata_add_value(meta, meta_charset, charset);
}

void
chupa_data_finished(ChupaData *data, GError *error)
{
    g_signal_emit(data, signals[FINISHED], 0, error);
}

gboolean
chupa_data_is_text(ChupaData *data)
{
    return chupa_utils_string_equal("text/plain", chupa_data_get_mime_type(data));
}

gboolean
chupa_data_is_succeeded(ChupaData *data)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    return priv->finished && priv->error == NULL;
}

gboolean
chupa_data_is_finished(ChupaData *data)
{
    return CHUPA_DATA_GET_PRIVATE(data)->finished;
}

GError *
chupa_data_get_error(ChupaData *data)
{
    return CHUPA_DATA_GET_PRIVATE(data)->error;
}

const gchar *
chupa_data_get_raw_data(ChupaData *data, gsize *length, GError **error)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    if (!priv->finished || priv->error) {
        if (length)
            *length = 0;
        return NULL;
    }

    if (!priv->raw_data) {
        GInputStream *input;
        gssize count;
        gchar buffer[1024];
        gsize buffer_size;

        priv->raw_data = g_string_new(NULL);
        input = G_INPUT_STREAM(priv->stream);
        buffer_size = sizeof(buffer);
        while ((count = g_input_stream_read(input, buffer, buffer_size,
                                            NULL, error)) > 0) {
            g_string_append_len(priv->raw_data, buffer, count);
            if (count < buffer_size)
                break;
        }
    }

    if (length)
        *length = priv->raw_data->len;
    return priv->raw_data->str;
}
