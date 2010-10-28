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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>

#include "chupa_logger.h"
#include "chupa_data_input.h"
#include <gsf/gsf-input-impl.h>

/* ChupaDataInput */
struct _ChupaDataInput
{
    GsfInput parent_instance;
};

struct _ChupaDataInputClass
{
    GsfInputClass parent_class;
};

G_DEFINE_TYPE(ChupaDataInput, chupa_data_input, GSF_INPUT_TYPE)

#define CHUPA_DATA_INPUT_GET_PRIVATE(obj)                       \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                         \
                                 CHUPA_TYPE_DATA_INPUT,         \
                                 ChupaDataInputPrivate))

typedef struct _ChupaDataInputPrivate ChupaDataInputPrivate;

struct _ChupaDataInputPrivate
{
    ChupaData *data;
    GString *buffer;
};

enum {
    PROP_0,
    PROP_DATA
};

static void
chupa_data_input_init(ChupaDataInput *data)
{
    ChupaDataInputPrivate *priv;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(data);
    priv->data = NULL;
    priv->buffer = g_string_new(NULL);
}

static void
constructed(GObject *object)
{
    GsfInput *input;
    ChupaDataInputPrivate *priv;
    ChupaMetadata *metadata;
    gsize content_length;

    input = GSF_INPUT(object);
    priv = CHUPA_DATA_INPUT_GET_PRIVATE(object);
    metadata = chupa_data_get_metadata(priv->data);
    content_length = chupa_metadata_get_content_length(metadata);
    if (content_length > 0) {
        gsf_input_set_size(input, content_length);
    } else {
        GInputStream *stream;

        stream = chupa_data_get_stream(priv->data);
        if (G_IS_SEEKABLE(stream)) {
            GSeekable *seekable;
            GError *error = NULL;

            seekable = G_SEEKABLE(stream);
            g_seekable_seek(seekable, 0, G_SEEK_END, NULL, &error);
            if (error) {
                chupa_error("failed to seek to the end "
                            "to determine content length: %s",
                            error->message);
                g_error_free(error);
            } else {
                content_length = g_seekable_tell(seekable);
                gsf_input_set_size(input, content_length);
                g_seekable_seek(seekable, 0, G_SEEK_SET, NULL, &error);
                if (error) {
                    chupa_error("failed to seek to the head "
                                "to determine content length: %s",
                                error->message);
                    g_error_free(error);
                }
            }
        }
    }
}

static void
dispose(GObject *object)
{
    ChupaDataInputPrivate *priv;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(object);

    if (priv->data) {
        g_object_unref(priv->data);
        priv->data = NULL;
    }

    if (priv->buffer) {
        g_string_free(priv->buffer, FALSE);
        priv->buffer = NULL;
    }

    G_OBJECT_CLASS(chupa_data_input_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaDataInputPrivate *priv;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_DATA:
        if (priv->data)
            g_object_unref(priv->data);
        priv->data = CHUPA_DATA(g_value_dup_object(value));
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
    ChupaDataInputPrivate *priv;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_DATA:
        g_value_set_object(value, priv->data);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static GsfInput *
input_dup(GsfInput *source_input, GError **error)
{
    ChupaDataInputPrivate *priv;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(source_input);
    return chupa_data_input_new(priv->data);
}

static guint8 const *
input_read(GsfInput *input, size_t num_bytes, guint8 *buffer)
{
    ChupaDataInputPrivate *priv;
    GInputStream *stream;
    gsize rest, position = 0;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(input);
    stream = chupa_data_get_stream(priv->data);
    if (priv->buffer->len < num_bytes)
        g_string_set_size(priv->buffer, num_bytes);
    rest = num_bytes;
    while (rest > 0) {
        gsize count;
        GError *error = NULL;

        count = g_input_stream_read(stream, priv->buffer->str + position,
                                    rest, NULL, &error);
        if (error) {
            chupa_error("failed to read: %s", error->message);
            g_error_free(error);
            return NULL;
        }
        if (count == 0)
            break;
        rest -= count;
        position += count;
    }

    if (buffer)
        memcpy(buffer, priv->buffer->str, position);

    return (guint8 const *)priv->buffer->str;
}

static gboolean
input_seek(GsfInput *input, gsf_off_t offset, GSeekType whence)
{
    ChupaDataInputPrivate *priv;
    GInputStream *stream;

    priv = CHUPA_DATA_INPUT_GET_PRIVATE(input);
    stream = chupa_data_get_stream(priv->data);
    while (stream) {
        GFilterInputStream *filter_stream;

        if (G_IS_SEEKABLE(stream)) {
            GSeekable *seekable;
            GError *error = NULL;

            seekable = G_SEEKABLE(stream);
            g_seekable_seek(seekable, offset, whence, NULL, &error);
            if (error) {
                chupa_error("failed to seek: %s", error->message);
                g_error_free(error);
                return TRUE;
            } else {
                return FALSE;
            }
        }

        if (!G_IS_FILTER_INPUT_STREAM(stream))
            break;

        filter_stream = G_FILTER_INPUT_STREAM(stream);
        stream = g_filter_input_stream_get_base_stream(filter_stream);
    }

    return TRUE;
}

static void
chupa_data_input_class_init(ChupaDataInputClass *klass)
{
    GObjectClass *gobject_class;
    GsfInputClass *input_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);
    input_class = GSF_INPUT_CLASS(klass);

    gobject_class->constructed  = constructed;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    input_class->Dup  = input_dup;
    input_class->Read = input_read;
    input_class->Seek = input_seek;

    spec = g_param_spec_object("data",
                               "Data",
                               "The data of input",
                               CHUPA_TYPE_DATA,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property(gobject_class, PROP_DATA, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaDataInputPrivate));
}

GsfInput *
chupa_data_input_new(ChupaData *data)
{
    return g_object_new(CHUPA_TYPE_DATA_INPUT,
                        "data", data,
                        NULL);
}
