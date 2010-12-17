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
chupa_test_setup(const gchar *type)
{
    cut_set_fixture_data_dir(cut_get_source_directory(), "fixtures", type, NULL);
}

void
chupa_test_teardown(void)
{
}

#define TAKE_OBJECT(obj) gcut_take_object(G_OBJECT(obj))
#define TAKE_STRING(str) cut_take_string(str)

const gchar *
chupa_test_decompose_data(const gchar *text, gsize size)
{
    ChupaData *data;

    data = chupa_test_decomposer_from_data(text, size);
    return chupa_test_decompose_all(data);
}

ChupaData *
chupa_test_decomposer_from_data(const gchar *text, gsize size)
{
    ChupaData *data;
    GInputStream *memory;

    TAKE_OBJECT(memory = g_memory_input_stream_new_from_data(text, size, NULL));
    TAKE_OBJECT(data = chupa_data_new(memory, NULL));
    return data;
}

const gchar *
chupa_test_decompose_fixture(const gchar *fixture)
{
    ChupaData *data;

    data = chupa_test_decomposer_from_fixture(fixture);
    return chupa_test_decompose_all(data);
}

ChupaData *
chupa_test_decomposer_from_fixture(const gchar *fixture)
{
    GError *error = NULL;
    ChupaData *data;
    gchar *sample_path;
    GFile *sample_file;

    TAKE_STRING(sample_path = cut_build_fixture_data_path(fixture, NULL));
    TAKE_OBJECT(sample_file = g_file_new_for_path(sample_path));
    TAKE_OBJECT(data = chupa_data_new_from_file(sample_file, NULL, &error));
    gcut_assert_error(error);
    return data;
}

const gchar *
chupa_test_decompose_all(ChupaData *data)
{
    GError *error = NULL;
    ChupaFeeder *feeder;
    ChupaData *text_data;
    const gchar *text;

    if (!data) {
        return NULL;
    }
    feeder = chupa_feeder_new();
    TAKE_OBJECT(feeder);
    text_data = chupa_feeder_decompose(feeder, data, &error);
    gcut_assert_error(error);

    text = chupa_data_get_raw_data(text_data, NULL, &error);
    gcut_assert_error(error);

    return text;
}

static void
cb_accepted(ChupaFeeder *feeder, ChupaData *data, gpointer user_data)
{
    ChupaMetadata **metadata = user_data;

    *metadata = chupa_data_get_metadata(data);
}

ChupaMetadata *
chupa_test_decompose_metadata(ChupaData *data)
{
    GError *error = NULL;
    ChupaFeeder *feeder;
    ChupaMetadata *metadata = NULL;

    if (!data) {
        return NULL;
    }
    feeder = CHUPA_FEEDER(TAKE_OBJECT(chupa_feeder_new()));
    g_signal_connect(feeder, "accepted", (GCallback)cb_accepted, &metadata);
    chupa_feeder_feed(feeder, data, &error);
    gcut_assert_error(error);

    return metadata;
}

ChupaMetadata *
chupa_test_metadata_fixture(const gchar *fixture)
{
    ChupaData *data;

    data = chupa_test_decomposer_from_fixture(fixture);
    return chupa_test_decompose_metadata(data);
}
