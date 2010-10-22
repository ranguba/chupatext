/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#include "chupa_memory_input_stream.h"
#include <string.h>

struct _ChupaMemoryInputStream
{
    GMemoryInputStream parent_object;
    GsfOutputMemory *source;
};

struct _ChupaMemoryInputStreamClass
{
    GMemoryInputStreamClass parent_class;
};

G_DEFINE_TYPE(ChupaMemoryInputStream,           \
              chupa_memory_input_stream,        \
              G_TYPE_MEMORY_INPUT_STREAM)

static void
dispose(GObject *object)
{
    ChupaMemoryInputStream *stream;

    stream = CHUPA_MEMORY_INPUT_STREAM(object);
    if (stream->source) {
        g_object_unref(stream->source);
        stream->source = NULL;
    }

    G_OBJECT_CLASS(chupa_memory_input_stream_parent_class)->dispose(object);
}

static void
chupa_memory_input_stream_init(ChupaMemoryInputStream *stream)
{
    stream->source = NULL;
}

static void
chupa_memory_input_stream_class_init(ChupaMemoryInputStreamClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = dispose;
}

GInputStream *
chupa_memory_input_stream_new(GsfOutputMemory *source)
{
    ChupaMemoryInputStream *stream;

    stream = g_object_new(CHUPA_TYPE_MEMORY_INPUT_STREAM, NULL);
    stream->source = g_object_ref(source);
    g_memory_input_stream_add_data(G_MEMORY_INPUT_STREAM(stream),
                                   gsf_output_memory_get_bytes(stream->source),
                                   gsf_output_size(GSF_OUTPUT(stream->source)),
                                   NULL);
    return G_INPUT_STREAM(stream);
}
