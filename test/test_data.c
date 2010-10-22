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

#include <gsf/gsf-input-memory.h>
#include <chupatext/chupa_data.h>
#include <chupatext/chupa_feeder.h>

#include <gcutter.h>

void test_new (void);
void test_new_with_metadata (void);
void test_is_text (void);
void test_is_succeeded (void);
void test_is_finished (void);

static ChupaData *data;
static GsfInput *memory_input;
static GInputStream *stream;

void
setup (void)
{
    data = NULL;
    memory_input = NULL;
    stream = NULL;
}

void
teardown (void)
{
    if (data)
        g_object_unref(data);
    if (memory_input)
        g_object_unref(memory_input);
    if (stream)
        g_object_unref(stream);
}

void
test_new (void)
{
    ChupaMetadata *meta;

    stream = g_memory_input_stream_new();
    data = chupa_data_new(stream, NULL);
    meta = chupa_data_get_metadata(data);
    cut_assert_not_null(meta);
    cut_assert_equal_int(1, chupa_metadata_size(meta));
    cut_assert_not_null(chupa_metadata_get_first_value(meta, "mime-type"));
}

void
test_new_with_metadata (void)
{
    ChupaMetadata *metadata = chupa_metadata_new();

    stream = g_memory_input_stream_new();
    data = chupa_data_new(stream, metadata);
    gcut_assert_equal_object(metadata, chupa_data_get_metadata(data));
    cut_assert_equal_uint(1, chupa_metadata_size(metadata));
}

void
test_is_text (void)
{
    const char text_data[] = "plain\n\0text\nfoo\0bar";
    ChupaMetadata *metadata;

    stream = g_memory_input_stream_new_from_data(text_data,
                                                 sizeof(text_data) - 1,
                                                 NULL);
    metadata = chupa_metadata_new();
    data = chupa_data_new(stream, metadata);
    g_object_unref(metadata);

    cut_assert_false(chupa_data_is_text(data));
    chupa_data_set_mime_type(data, "text/plain");
    cut_assert_true(chupa_data_is_text(data));
}

void
test_is_succeeded (void)
{
    const char text_data[] = "plain\n\0text\nfoo\0bar";

    stream = g_memory_input_stream_new_from_data(text_data,
                                                 sizeof(text_data) - 1,
                                                 NULL);
    data = chupa_data_new(stream, NULL);

    cut_assert_false(chupa_data_is_succeeded(data));
    chupa_data_finished(data, NULL);
    cut_assert_true(chupa_data_is_succeeded(data));
}

void
test_is_finished (void)
{
    const char text_data[] = "plain\n\0text\nfoo\0bar";
    GError *error;

    stream = g_memory_input_stream_new_from_data(text_data,
                                                 sizeof(text_data) - 1,
                                                 NULL);
    data = chupa_data_new(stream, NULL);

    cut_assert_false(chupa_data_is_succeeded(data));
    cut_assert_false(chupa_data_is_finished(data));
    error = g_error_new(CHUPA_FEEDER_ERROR,
                        CHUPA_FEEDER_ERROR_UNKNOWN_MIME_TYPE,
                        "unknown mime-type: <image/png>");
    chupa_data_finished(data, error);
    g_error_free(error);
    cut_assert_false(chupa_data_is_succeeded(data));
    cut_assert_true(chupa_data_is_finished(data));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
