/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>
#include <cutter/cut-run-context.h>

static ChupaText *chupar;
static GInputStream *source;
static gchar *sample_path;
static GFile *sample_file;
static gchar *read_data;

#if !CUTTER_CHECK_VERSION(1, 1, 5)
#define cut_get_source_directory() \
    cut_run_context_get_source_directory( \
      cut_test_context_get_run_context( \
          cut_get_current_test_context()))
#endif

void
setup(void)
{
    chupa_decomposer_load_modules();

    chupar = NULL;
    source = NULL;
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
    if (source)
        g_object_unref(source);
    if (sample_path)
        g_free(sample_path);
    if (sample_file)
        g_object_unref(sample_file);
    if (read_data)
        g_free(read_data);
}

static void
text_decomposed(gpointer obj, gpointer arg, gpointer udata)
{
    GDataInputStream *data = G_DATA_INPUT_STREAM(arg);
    gsize length;

    *(gpointer *)udata = g_data_input_stream_read_until(data, "", &length, NULL, NULL);
}

void
test_decompose_msword(void)
{
    chupar = chupa_text_new();
    g_signal_connect(chupar, chupa_text_signal_decomposed, (GCallback)text_decomposed, &read_data);
    sample_path = cut_build_fixture_data_path("sample.doc", NULL);
    sample_file = g_file_new_for_path(sample_path);
    source = G_INPUT_STREAM(g_file_read(sample_file, NULL, NULL));
    chupa_text_feed(chupar, source);
    cut_assert_equal_string("sample\n", read_data);
}
