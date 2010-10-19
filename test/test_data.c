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

void
setup (void)
{
    data = NULL;
    memory_input = NULL;
}

void
teardown (void)
{
    if (data)
        g_object_unref(data);
    if (memory_input)
        g_object_unref(memory_input);
}

void
test_new (void)
{
    ChupaMetadata *meta;
    GInputStream *memstream = g_memory_input_stream_new();
    data = chupa_data_new_from_stream(NULL, memstream, NULL);
    meta = chupa_data_get_metadata(data);
    cut_assert_not_null(meta);
    cut_assert_equal_int(1, chupa_metadata_size(meta));
    cut_assert_not_null(chupa_metadata_get_first_value(meta, "mime-type"));
}

void
test_new_with_metadata (void)
{
    ChupaMetadata *meta = chupa_metadata_new();
    GInputStream *memstream = g_memory_input_stream_new();
    data = chupa_data_new_from_stream(meta, memstream, NULL);
    gcut_assert_equal_object(meta, chupa_data_get_metadata(data));
    cut_assert_equal_uint(1, chupa_metadata_size(meta));
}

void
test_is_text (void)
{
    const char text_data[] = "plain\n\0text\nfoo\0bar";
    ChupaMetadata *meta;

    memory_input = GSF_INPUT(gsf_input_memory_new((guint8 const *) text_data,
                                                  sizeof(text_data) - 1,
                                                  FALSE));
    meta = chupa_metadata_new();
    data = chupa_data_new(meta, memory_input);

    cut_assert_false(chupa_data_is_text(data));
    chupa_data_set_mime_type(data, "text/plain");
    cut_assert_true(chupa_data_is_text(data));
}

void
test_is_succeeded (void)
{
    const char text_data[] = "plain\n\0text\nfoo\0bar";

    memory_input = GSF_INPUT(gsf_input_memory_new((guint8 const *) text_data,
                                                  sizeof(text_data) - 1,
                                                  FALSE));
    data = chupa_data_new(NULL, memory_input);

    cut_assert_false(chupa_data_is_succeeded(data));
    chupa_data_finished(data, NULL);
    cut_assert_true(chupa_data_is_succeeded(data));
}

void
test_is_finished (void)
{
    const char text_data[] = "plain\n\0text\nfoo\0bar";
    GError *error;

    memory_input = GSF_INPUT(gsf_input_memory_new((guint8 const *) text_data,
                                                  sizeof(text_data) - 1,
                                                  FALSE));
    data = chupa_data_new(NULL, memory_input);

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
