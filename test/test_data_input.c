/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

#include <gsf/gsf-input-memory.h>
#include <chupatext/chupa_data_input.h>

#include <gcutter.h>

void test_read(void);

static GInputStream *memory_stream;
static ChupaData *data;
static ChupaMetadata *metadata;
static GsfInput *input;

void
setup (void)
{
    memory_stream = NULL;
    data = NULL;
    metadata = NULL;
    input = NULL;
}

void
teardown (void)
{
    if (memory_stream)
        g_object_unref(memory_stream);
    if (data)
        g_object_unref(data);
    if (metadata)
        g_object_unref(metadata);
    if (input)
        g_object_unref(input);
}

void
test_read(void)
{
    const char chunk[] = "plain\n\0text\nfoo\0bar";
    const guint8 *actual;
    gsize length;
    GString *content_length_string;

    length = sizeof(chunk) - 1;

    memory_stream = g_memory_input_stream_new_from_data(chunk, length, NULL);
    content_length_string = g_string_new(NULL);
    g_string_printf(content_length_string, "%zd", length);
    metadata = chupa_metadata_new();
    chupa_metadata_set_string(metadata,
                              "content-length", content_length_string->str);
    g_string_free(content_length_string, TRUE);
    data = chupa_data_new(memory_stream, metadata);
    input = chupa_data_input_new(data);
    actual = gsf_input_read(input, length, NULL);
    cut_assert_not_null(actual);
    cut_assert_equal_memory(chunk, length, actual, length);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
