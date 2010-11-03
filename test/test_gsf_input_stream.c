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

#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-input-impl.h>
#include <chupatext/chupa_gsf_input_stream.h>

#include <gcutter.h>

void test_new (void);
void test_new_with_metadata (void);
void test_read(void);
void test_get_metadata(void);

static GsfInput *memory_input;
static ChupaGsfInputStream *stream;
static GDataInputStream *data_input;
static ChupaMetadata *metadata;

void
setup (void)
{
    memory_input = NULL;
    stream = NULL;
    data_input = NULL;
    metadata = NULL;
}

void
teardown (void)
{
    if (memory_input)
        g_object_unref(memory_input);
    if (stream)
        g_object_unref(stream);
    if (data_input)
        g_object_unref(data_input);
    if (metadata)
        g_object_unref(metadata);
}

void
test_read(void)
{
    const gchar data[] = "plain\n\0text\nfoo\0bar";
    gsize length;
    const gchar *str;

    memory_input = GSF_INPUT(gsf_input_memory_new((guint8 const *) data, sizeof(data) - 1, FALSE));
    stream = chupa_gsf_input_stream_new(memory_input);
    data_input = g_data_input_stream_new(G_INPUT_STREAM(stream));
    cut_assert_not_null(data_input);
    str = cut_take_string(g_data_input_stream_read_line(data_input, &length, NULL, NULL));
    cut_assert_equal_string("plain", str);
    cut_assert_equal_uint(5, length);
    cut_assert_equal_uint(0, g_data_input_stream_read_byte(data_input, NULL, NULL));
    str = cut_take_string(g_data_input_stream_read_line(data_input, &length, NULL, NULL));
    cut_assert_equal_string("text", str);
    cut_assert_equal_uint(4, length);
    str = cut_take_memory(g_data_input_stream_read_line(data_input, &length, NULL, NULL));
    cut_assert_equal_memory("foo\0bar", 7, str, length);
}

void
test_get_metadata(void)
{
    const gchar data[] = "foobar";

    memory_input = GSF_INPUT(gsf_input_memory_new((guint8 const *) data, sizeof(data) - 1, FALSE));
    gsf_input_set_name_from_filename(memory_input, "memory_input.dat");

    stream = chupa_gsf_input_stream_new(memory_input);
    cut_assert_not_null(stream);
    metadata = chupa_gsf_input_stream_get_metadata(stream);
    cut_assert_not_null(metadata);
    cut_assert_equal_size(6, chupa_metadata_get_content_length(metadata));
    cut_assert_equal_string("memory_input.dat",
                            chupa_metadata_get_string(metadata, "filename", NULL));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
