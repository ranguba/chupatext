/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_gsf_input_stream.h"

struct _ChupaGsfInputStream
{
    GMemoryInputStream parent_object;
    GsfOutputMemory *source;
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
