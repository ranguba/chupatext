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

#include <chupatext/chupa_text_input_stream.h>

#include <gcutter.h>

void test_new (void);
void test_new_with_metadata (void);
void test_read(void);

static GInputStream *memstream;
static ChupaTextInput *input;

void
setup (void)
{
    memstream = NULL;
    input = NULL;
}

void
teardown (void)
{
    if (memstream)
        g_object_unref(memstream);
    if (input)
        g_object_unref(input);
}

void
test_read(void)
{
    static const char data[] = "plain\n\0text\nfoo\0bar";
    GDataInputStream *dis;
    gsize length;
    const char *str;

    memstream = g_memory_input_stream_new_from_data(data, sizeof(data) - 1, NULL);
    input = chupa_text_input_new_from_stream(NULL, memstream, NULL);
    dis = G_DATA_INPUT_STREAM(chupa_text_input_get_stream(input));
    cut_assert_not_null(dis);
    str = cut_take_string(g_data_input_stream_read_line(dis, &length, NULL, NULL));
    cut_assert_equal_string("plain", str);
    cut_assert_equal_uint(5, length);
    cut_assert_equal_uint(0, g_data_input_stream_read_byte(dis, NULL, NULL));
    str = cut_take_string(g_data_input_stream_read_line(dis, &length, NULL, NULL));
    cut_assert_equal_string("text", str);
    cut_assert_equal_uint(4, length);
    str = cut_take_memory(g_data_input_stream_read_line(dis, &length, NULL, NULL));
    cut_assert_equal_memory("foo\0bar", 7, str, length);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
