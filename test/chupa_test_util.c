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

#include "chupa_test_util.h"

void
chupa_test_setup(void)
{
    cut_set_fixture_data_dir(cut_get_source_directory(), "fixtures", NULL);
}

void
chupa_test_teardown(void)
{
}

#define TAKE_OBJECT(obj) gcut_take_object(G_OBJECT(obj))
#define TAKE_STRING(str) cut_take_string(str)

const gchar *
chupa_test_decompose_data(const char *text, gsize size, GError **error)
{
    ChupaData *data = chupa_test_decomposer_from_data(text, size, error);
    return chupa_test_decompose_all(data, error);
}

ChupaData *
chupa_test_decomposer_from_data(const char *text, gsize size, GError **error)
{
    ChupaData *data;
    GInputStream *mem;

    TAKE_OBJECT(mem = g_memory_input_stream_new_from_data(text, size, NULL));
    TAKE_OBJECT(data = chupa_data_new_from_stream(NULL, mem, NULL));
    return data;
}

const gchar *
chupa_test_decompose_fixture(const char *fixture, GError **error)
{
    ChupaData *data = chupa_test_decomposer_from_fixture(fixture, error);
    return chupa_test_decompose_all(data, error);
}

ChupaData *
chupa_test_decomposer_from_fixture(const char *fixture, GError **error)
{
    ChupaData *data;
    gchar *sample_path;
    GFile *sample_file;

    TAKE_STRING(sample_path = cut_build_fixture_data_path(fixture, NULL));
    TAKE_OBJECT(sample_file = g_file_new_for_path(sample_path));
    TAKE_OBJECT(data = chupa_data_new_from_file(NULL, sample_file,
                                                            error));
    return data;
}

const gchar *
chupa_test_decompose_all(ChupaData *data, GError **error)
{
    ChupaFeeder *feeder;
    ChupaData *text_data;
    GInputStream *input;
    GString *text;
    gssize count;
    gchar buffer[1024];
    gsize buffer_size;

    if (!data) {
        return NULL;
    }
    feeder = chupa_feeder_new();
    TAKE_OBJECT(feeder);
    text_data = chupa_feeder_decompose(feeder, data, error);
    if (!text_data) {
        return NULL;
    }

    text = g_string_new(NULL);
    input = chupa_data_get_stream(text_data);
    buffer_size = sizeof(buffer);
    while ((count = g_input_stream_read(input, buffer, buffer_size,
                                        NULL, error)) > 0) {
        g_string_append_len(text, buffer, count);
        if (count < buffer_size)
            break;
    }
    g_object_unref(text_data);
    return TAKE_STRING(g_string_free(text, FALSE));
}

static void
decomposed_metadata(ChupaFeeder *feeder, ChupaData *data, gpointer user_data)
{
    ChupaMetadata **metadata = user_data;

    *metadata = chupa_data_get_metadata(data);
}

ChupaMetadata *
chupa_test_decompose_metadata(ChupaData *data, GError **error)
{
    ChupaFeeder *feeder;
    ChupaMetadata *metadata = NULL;

    if (!data) {
        return NULL;
    }
    feeder = CHUPA_FEEDER(TAKE_OBJECT(chupa_feeder_new()));
    g_signal_connect(feeder, "decomposed",
                     (GCallback)decomposed_metadata, &metadata);
    chupa_feeder_feed(feeder, data, error);
    TAKE_OBJECT(metadata);
    return metadata;
}

ChupaMetadata *
chupa_test_metadata_fixture(const char *fixture, GError **error)
{
    ChupaData *data = chupa_test_decomposer_from_fixture(fixture, error);
    return chupa_test_decompose_metadata(data, error);
}
