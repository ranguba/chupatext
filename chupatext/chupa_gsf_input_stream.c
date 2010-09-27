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

#include "chupatext/chupa_gsf_input_stream.h"
#include <string.h>

typedef struct _ChupaGsfInputStreamPrivate ChupaGsfInputStreamPrivate;

struct _ChupaGsfInputStream
{
    GMemoryInputStream parent_object;
    GsfOutputMemory *source;
    gsize cur_offset;
};

struct _ChupaGsfInputStreamClass
{
    GMemoryInputStreamClass parent_class;
};

G_DEFINE_TYPE(ChupaGsfInputStream, chupa_gsf_input_stream, G_TYPE_MEMORY_INPUT_STREAM)

static void
gsf_input_dispose(GObject *object)
{
    ChupaGsfInputStream *stream = CHUPA_GSF_INPUT_STREAM(object);

    if (stream->source) {
        g_object_unref(stream->source);
        stream->source = NULL;
    }

    G_OBJECT_CLASS(chupa_gsf_input_stream_parent_class)->dispose(object);
}

static gssize
gsf_input_stream_read(GInputStream *input_stream,
                      void *buffer,
                      gsize count,
                      GCancellable *cancellable,
                      GError **error)
{
    ChupaGsfInputStream *stream = CHUPA_GSF_INPUT_STREAM(input_stream);
    gsize cur_size;

    g_return_val_if_fail(stream->source, -1);
    g_return_val_if_fail(count >= 0, -1);
    cur_size = gsf_output_size(GSF_OUTPUT(stream->source));
    if (count > cur_size) {
        return -1;
    }
    if (count + stream->cur_offset > cur_size) {
        count = cur_size - stream->cur_offset;
    }
    if (count > 0) {
        const guint8 *mem_buffer = gsf_output_memory_get_bytes(stream->source);
        memcpy(buffer, mem_buffer + stream->cur_offset, count);
    }
    return count;
}

static void
chupa_gsf_input_stream_init(ChupaGsfInputStream *stream)
{
    stream->source = NULL;
}

static void
chupa_gsf_input_stream_class_init(ChupaGsfInputStreamClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose      = gsf_input_dispose;
    {
        GInputStreamClass *input_stream_class = G_INPUT_STREAM_CLASS(klass);
        input_stream_class->read_fn = gsf_input_stream_read;
    }
}

GInputStream *
chupa_gsf_input_stream_new(GsfOutputMemory *mem)
{
    ChupaGsfInputStream *stream = g_object_new(CHUPA_TYPE_GSF_INPUT_STREAM, NULL);
    GsfOutput *out = GSF_OUTPUT(mem);
    gsize size;

    g_object_ref(mem);
    stream->source = mem;
    size = gsf_output_size(out);
    if (size > 0) {
        g_memory_input_stream_add_data(G_MEMORY_INPUT_STREAM(stream),
                                       gsf_output_memory_get_bytes(mem),
                                       size, NULL);
    }
    return G_INPUT_STREAM(stream);
}
