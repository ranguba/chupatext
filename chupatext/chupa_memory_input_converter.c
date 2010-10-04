/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; coding: us-ascii -*- */
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

#include <chupatext/chupa_memory_input_converter.h>

typedef struct _ChupaMemoryInputConverterPrivate ChupaMemoryInputConverterPrivate;
#define CHUPA_MEMORY_INPUT_CONVERTER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_MEMORY_INPUT_CONVERTER, \
                                 ChupaMemoryInputConverterPrivate))

static void chupa_memory_input_converter_class_init(ChupaMemoryInputConverterClass *klass);
static void converter_iface_init(GConverterIface *converter_iface);

G_DEFINE_ABSTRACT_TYPE_WITH_CODE(ChupaMemoryInputConverter,
                                 chupa_memory_input_converter,
                                 CHUPA_TYPE_MEMORY_INPUT_CONVERTER,
                                 G_IMPLEMENT_INTERFACE(G_TYPE_CONVERTER,
                                                       converter_iface_init))

struct _ChupaMemoryInputConverterPrivate {
    GsfOutputMemory *storage;
};

static GConverterResult
converter_convert(GConverter *converter,
                  const void *inbuf,
                  gsize       inbuf_size,
                  void       *outbuf,
                  gsize       outbuf_size,
                  GConverterFlags flags,
                  gsize      *bytes_read,
                  gsize      *bytes_written,
                  GError    **error)
{
    ChupaMemoryInputConverterClass *klass;
    ChupaMemoryInputConverterPrivate *priv;

    g_return_val_if_fail(converter, FALSE);
    g_return_val_if_fail(inbuf, FALSE);

    priv = CHUPA_MEMORY_INPUT_CONVERTER_GET_PRIVATE(converter);
    if (!priv->storage) {
        priv->storage = GSF_OUTPUT_MEMORY(gsf_output_memory_new());
    }
    if (!gsf_output_write((GsfOutput *)priv->storage, inbuf_size, inbuf)) {
        return G_CONVERTER_ERROR;
    }
    if (bytes_read) {
        *bytes_read = inbuf_size;
    }
    if (bytes_written) {
        *bytes_written = 0;
    }
    if (flags & G_CONVERTER_INPUT_AT_END) {
        if (flags & G_CONVERTER_FLUSH) {
            return G_CONVERTER_FLUSHED;
        }
        return G_CONVERTER_CONVERTED;
    }
    klass = CHUPA_MEMORY_INPUT_CONVERTER_GET_CLASS(converter);
    return klass->convert_output(CHUPA_MEMORY_INPUT_CONVERTER(converter),
                                 gsf_output_memory_get_bytes(priv->storage),
                                 gsf_output_size((GsfOutput *)priv->storage),
                                 outbuf, outbuf_size,
                                 G_CONVERTER_INPUT_AT_END,
                                 bytes_written,
                                 error);
}

static void
converter_reset(GConverter *converter)
{
}

static void
chupa_memory_input_converter_init(ChupaMemoryInputConverter *converter)
{
    CHUPA_MEMORY_INPUT_CONVERTER_GET_PRIVATE(converter)->storage = NULL;
}

static void
chupa_memory_input_converter_class_init(ChupaMemoryInputConverterClass *klass)
{
}

static void
converter_iface_init(GConverterIface *converter_iface)
{
    converter_iface->convert = converter_convert;
    converter_iface->reset = converter_reset;
}

