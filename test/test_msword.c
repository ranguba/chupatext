/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>
#include <cutter/cut-run-context.h>

static ChupaText *chupar;
static ChupaTextInput *text_input;
static GsfInput *sample_input;
static gchar *sample_path;
static GFile *sample_file;
static gchar *read_data;

void
setup(void)
{
    chupa_decomposer_load_modules();

    chupar = NULL;
    text_input = NULL;
    sample_input = NULL;
    sample_path = NULL;
    sample_file = NULL;
    read_data = NULL;

    cut_set_fixture_data_dir(cut_get_source_directory(), "fixtures", NULL);
}

void
teardown(void)
{
    if (chupar)
        g_object_unref(chupar);
    if (text_input)
        g_object_unref(text_input);
    if (sample_input)
        g_object_unref(sample_input);
    if (sample_path)
        g_free(sample_path);
    if (sample_file)
        g_object_unref(sample_file);
    if (read_data)
        g_free(read_data);
}

static void
text_decomposed(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
{
    GDataInputStream *data = G_DATA_INPUT_STREAM(chupa_text_input_get_stream(input));
    gsize length;

    *(gpointer *)udata = g_data_input_stream_read_until(data, "", &length, NULL, NULL);
}

void
test_decompose_msword(void)
{
    chupar = chupa_text_new();
    chupa_text_connect_decomposed(chupar, text_decomposed, &read_data);
    sample_path = cut_build_fixture_data_path("sample.doc", NULL);
    sample_file = g_file_new_for_path(sample_path);
    text_input = chupa_text_input_new_from_file(NULL, sample_file);
    chupa_text_feed(chupar, text_input);
    cut_assert_equal_string("sample\n", read_data);
}
