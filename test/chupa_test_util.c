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

char *
chupa_test_decompose_data(const char *text, gsize size, GError **err)
{
    ChupaTextInput *input = chupa_test_decomposer_from_data(text, size, err);
    return chupa_test_decompose_all(input, err);
}

ChupaTextInput *
chupa_test_decomposer_from_data(const char *text, gsize size, GError **err)
{
    ChupaTextInput *text_input;
    GInputStream *mem;

    TAKE_OBJECT(mem = g_memory_input_stream_new_from_data(text, size, NULL));
    TAKE_OBJECT(text_input = chupa_text_input_new_from_stream(NULL, mem, NULL));
    return text_input;
}

char *
chupa_test_decompose_fixture(const char *fixture, GError **err)
{
    ChupaTextInput *input = chupa_test_decomposer_from_fixture(fixture, err);
    return chupa_test_decompose_all(input, err);
}

ChupaTextInput *
chupa_test_decomposer_from_fixture(const char *fixture, GError **err)
{
    ChupaTextInput *text_input;
    gchar *sample_path;
    GFile *sample_file;

    TAKE_STRING(sample_path = cut_build_fixture_data_path(fixture, NULL));
    TAKE_OBJECT(sample_file = g_file_new_for_path(sample_path));
    TAKE_OBJECT(text_input = chupa_text_input_new_from_file(NULL, sample_file, err));
    return text_input;
}

char *
chupa_test_decompose_all(ChupaTextInput *text_input, GError **err)
{
    ChupaText *chupar;
    char *str;

    if (!text_input) {
        return NULL;
    }
    TAKE_OBJECT(chupar = chupa_text_new());
    TAKE_STRING(str = chupa_text_decompose_all(chupar, text_input, err));
    return str;
}

static void
decomposed_metadata(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
{
    *(ChupaMetadata **)udata = chupa_text_input_get_metadata(input);
}

ChupaMetadata *
chupa_test_decompose_metadata(ChupaTextInput *text_input, GError **err)
{
    ChupaText *chupar;
    ChupaMetadata *metadata = NULL;

    if (!text_input) {
        return NULL;
    }
    TAKE_OBJECT(chupar = chupa_text_new());
    chupa_text_decompose(chupar, text_input, decomposed_metadata, &metadata, err);
    TAKE_OBJECT(metadata);
    return metadata;
}

ChupaMetadata *
chupa_test_metadata_fixture(const char *fixture, GError **err)
{
    ChupaTextInput *input = chupa_test_decomposer_from_fixture(fixture, err);
    return chupa_test_decompose_metadata(input, err);
}
