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

#include "chupa_logger.h"
#include "chupa_data.h"
#include "chupa_gsf_input_stream.h"
#include <gio/gio.h>

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
    ChupaMetadata *metadata;
    GInputStream *stream;
    gboolean finished;
    GError *error;
    GString *raw_data;
};

enum {
    PROP_0,
    PROP_STREAM,
    PROP_METADATA,
    PROP_DUMMY
};

enum {
    FINISHED,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

static gboolean
read_head_data(GInputStream *stream, guchar *buffer, gsize buffer_size,
               gsize *read_bytes)
{
    GSeekable *seekable;
    gsize original_position;
    GError *error = NULL;

    if (!G_IS_SEEKABLE(stream))
        return FALSE;

    seekable = G_SEEKABLE(stream);
    original_position = g_seekable_tell(seekable);
    g_seekable_seek(seekable, 0, G_SEEK_SET, NULL, &error);
    if (error) {
        chupa_error("failed to seek to the head to guess content-type: %s",
                    error->message);
        g_error_free(error);
        return FALSE;
    }

    *read_bytes = g_input_stream_read(stream, buffer, buffer_size, NULL, &error);
    if (error) {
        chupa_error("failed to read head data to guess content-type: %s",
                    error->message);
        g_error_free(error);
        g_seekable_seek(seekable, original_position, G_SEEK_SET, NULL, NULL);
        return FALSE;
    }

    g_seekable_seek(seekable, original_position, G_SEEK_SET, NULL, &error);
    if (error) {
        chupa_error("failed to re-seek to the original position "
                    "to reset position to guess content-type: %s",
                    error->message);
        g_error_free(error);
        return FALSE;
    }

    return TRUE;
}

static const gchar *
guess_mime_type(const char *name, GInputStream *stream, gboolean *uncertain)
{
    const char *content_type = NULL;
    guchar data[1024];
    gsize data_length;

    if (read_head_data(stream, data, sizeof(data), &data_length))
        content_type = g_content_type_guess(name, data, data_length, uncertain);

    if (!content_type)
        content_type = g_content_type_guess(name, NULL, 0, uncertain);

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
    const gchar *filename = NULL;

    priv = CHUPA_DATA_GET_PRIVATE(object);
    g_return_if_fail(priv->stream);

    if (!priv->metadata) {
        priv->metadata = chupa_metadata_new();
    }
    filename = chupa_metadata_get_string(priv->metadata, meta_filename, NULL);
    mime_type = guess_mime_type(filename, priv->stream, NULL);
    chupa_metadata_set_string(priv->metadata, "mime-type", mime_type);
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

    priv = CHUPA_DATA_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_STREAM:
        if (priv->stream)
            g_object_unref(priv->stream);
        priv->stream = g_value_dup_object(value);
        break;
    case PROP_METADATA:
        if (priv->metadata)
            g_object_unref(priv->metadata);
        priv->metadata = g_value_dup_object(value);
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

    spec = g_param_spec_object("stream",
                               "Stream",
                               "Stream of the input",
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

    signals[FINISHED] =
        g_signal_new("finished",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaDataClass, finished),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__BOOLEAN,
                     G_TYPE_NONE, 1, G_TYPE_BOOLEAN);

    g_type_class_add_private(gobject_class, sizeof(ChupaDataPrivate));
}

ChupaData *
chupa_data_new(GInputStream *stream, ChupaMetadata *metadata)
{
    return g_object_new(CHUPA_TYPE_DATA,
                        "stream", stream,
                        "metadata", metadata,
                        NULL);
}

ChupaData *
chupa_data_new_from_file(GFile *file, ChupaMetadata *metadata, GError **error)
{
    ChupaData *data;
    GInputStream *stream;
    GFileInfo *info;

    stream = G_INPUT_STREAM(g_file_read(file, NULL, error));
    if (!stream)
        return NULL;

    if (!metadata)
        metadata = chupa_metadata_new();

    info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_SIZE, 0,
                             NULL, NULL);
    if (info) {
        chupa_metadata_set_content_length(metadata, g_file_info_get_size(info));
        g_object_unref(info);
    }

    info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME, 0,
                             NULL, NULL);
    if (info) {
        chupa_metadata_set_string(metadata,
                                  meta_filename,
                                  g_file_info_get_display_name(info));
        g_object_unref(info);
    }

    if (!chupa_metadata_get_path(metadata)) {
        gchar *path;
        path = g_file_get_path(file);
        if (path) {
            chupa_metadata_set_path(metadata, path);
            g_free(path);
        }
    }

    data = chupa_data_new(stream, metadata);
    g_object_unref(stream);
    g_object_unref(metadata);
    return data;
}

GInputStream *
chupa_data_get_stream(ChupaData *data)
{
    return CHUPA_DATA_GET_PRIVATE(data)->stream;
}

ChupaMetadata *
chupa_data_get_metadata(ChupaData *data)
{
    return CHUPA_DATA_GET_PRIVATE(data)->metadata;
}

const gchar *
chupa_data_get_mime_type(ChupaData *data)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    return chupa_metadata_get_string(meta, "mime-type", NULL);
}

void
chupa_data_set_mime_type(ChupaData *data, const gchar *mime_type)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    chupa_metadata_set_string(meta, "mime-type", mime_type);
}

const gchar *
chupa_data_get_filename(ChupaData *data)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    return chupa_metadata_get_string(meta, meta_filename, NULL);
}

void
chupa_data_set_filename(ChupaData *data, const char *filename)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    chupa_metadata_set_string(meta, meta_filename, filename);
}

const gchar *
chupa_data_get_charset(ChupaData *data)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    return chupa_metadata_get_string(meta, meta_charset, NULL);
}

void
chupa_data_set_charset(ChupaData *data, const char *charset)
{
    ChupaMetadata *meta = chupa_data_get_metadata(data);
    chupa_metadata_set_string(meta, meta_charset, charset);
}

void
chupa_data_finished(ChupaData *data, GError *error)
{
    g_signal_emit(data, signals[FINISHED], 0, error);
}

gboolean
chupa_data_is_text(ChupaData *data)
{
    ChupaDataPrivate *priv;

    priv = CHUPA_DATA_GET_PRIVATE(data);
    if (!priv->metadata)
        return FALSE;

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
        gssize count;
        gchar buffer[1024];
        gsize buffer_size;

        priv->raw_data = g_string_new(NULL);
        buffer_size = sizeof(buffer);
        while ((count = g_input_stream_read(priv->stream, buffer, buffer_size,
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
