/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_test_util.h"

void
chupa_test_setup(void)
{
    chupa_decomposer_load_modules();

    cut_set_fixture_data_dir(cut_get_source_directory(), "fixtures", NULL);
}

void
chupa_test_teardown(void)
{
}

#define TAKE_OBJECT(obj) gcut_take_object(G_OBJECT(obj))
#define TAKE_STRING(str) cut_take_string(str)

char *
chupa_test_decompose_data(const char *text, gsize size)
{
    ChupaText *chupar;
    ChupaTextInput *text_input;
    GInputStream *mem;
    char *str;

    TAKE_OBJECT(chupar = chupa_text_new());
    TAKE_OBJECT(mem = g_memory_input_stream_new_from_data(text, size, NULL));
    TAKE_OBJECT(text_input = chupa_text_input_new_from_stream(NULL, mem, NULL));
    TAKE_STRING(str = chupa_text_decompose_all(chupar, text_input));
    return str;
}

char *
chupa_test_decompose_fixture(const char *fixture)
{
    ChupaText *chupar;
    ChupaTextInput *text_input;
    GsfInput *sample_input;
    gchar *sample_path;
    GFile *sample_file;
    char *str;

    TAKE_OBJECT(chupar = chupa_text_new());
    TAKE_STRING(sample_path = cut_build_fixture_data_path(fixture, NULL));
    TAKE_OBJECT(sample_file = g_file_new_for_path(sample_path));
    TAKE_OBJECT(text_input = chupa_text_input_new_from_file(NULL, sample_file));
    TAKE_STRING(str = chupa_text_decompose_all(chupar, text_input));
    return str;
}
