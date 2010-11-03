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

#include "chupa_gsf_input_stream.h"

G_DEFINE_TYPE(ChupaGsfInputStream, chupa_gsf_input_stream, G_TYPE_INPUT_STREAM)

#define CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
                                 CHUPA_TYPE_GSF_INPUT_STREAM, \
                                 ChupaGsfInputStreamPrivate))

typedef struct _ChupaGsfInputStreamPrivate ChupaGsfInputStreamPrivate;

struct _ChupaGsfInputStreamPrivate
{
    GsfInput *input;
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
chupa_gsf_input_stream_init(ChupaGsfInputStream *stream)
{
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(stream);
    priv->input = NULL;
}

static void
dispose(GObject *object)
{
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(object);
    if (priv->input) {
        g_object_unref(priv->input);
        priv->input = NULL;
    }
    G_OBJECT_CLASS(chupa_gsf_input_stream_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaGsfInputStreamPrivate *priv;
    GObject *obj;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        obj = g_value_dup_object(value);
        priv->input = GSF_INPUT(obj);
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
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(object);
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
    ChupaGsfInputStreamPrivate *priv;
    gsf_off_t remaining;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(stream);
    remaining = gsf_input_remaining(priv->input);
    if (count > remaining) {
        count = remaining;
    }
    if (!gsf_input_read(priv->input, count, buffer)) {
        return 0;
    }
    return count;
}

static gssize
skip_fn(GInputStream *stream, gsize count,
        GCancellable *cancellable, GError **error)
{
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(stream);
    if (gsf_input_seek(priv->input, count, G_SEEK_CUR)) {
        return (gssize)-1;
    }
    return gsf_input_tell(priv->input);
}

static gboolean
close_fn(GInputStream *stream, GCancellable *cancellable, GError **error)
{
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(stream);
    if (priv->input) {
        g_object_unref(priv->input);
        priv->input = NULL;
    }
    return TRUE;
}

static void
chupa_gsf_input_stream_class_init(ChupaGsfInputStreamClass *klass)
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
                               GSF_INPUT_TYPE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_INPUT, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaGsfInputStreamPrivate));
}

ChupaGsfInputStream *
chupa_gsf_input_stream_new(GsfInput *input)
{
    return g_object_new(CHUPA_TYPE_GSF_INPUT_STREAM,
                        "input", input,
                        NULL);
}

GsfInput *
chupa_gsf_input_stream_get_input(ChupaGsfInputStream *stream)
{
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(stream);
    return priv->input;
}

ChupaMetadata *
chupa_gsf_input_stream_get_metadata(ChupaGsfInputStream *stream)
{
    ChupaMetadata *metadata;
    ChupaGsfInputStreamPrivate *priv;

    priv = CHUPA_GSF_INPUT_STREAM_GET_PRIVATE(stream);
    metadata = chupa_metadata_new();
    chupa_metadata_set_content_length(metadata, gsf_input_size(priv->input));
    chupa_metadata_set_string(metadata, "filename", gsf_input_name(priv->input));

    return metadata;
}
