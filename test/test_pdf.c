/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>
#include <cutter/cut-run-context.h>

void
setup(void)
{
    chupa_decomposer_load_modules();

    cut_set_fixture_data_dir(cut_get_source_directory(), "fixtures", NULL);
}

void
teardown(void)
{
}

#define TAKE_OBJECT(obj) gcut_take_object(G_OBJECT(obj))
#define TAKE_STRING(str) cut_take_string(str)

static const char *
decompose_pdf(const char *fixture)
{
    ChupaText *chupar;
    ChupaTextInput *text_input;
    GsfInput *sample_input;
    gchar *sample_path;
    GFile *sample_file;

    TAKE_OBJECT(chupar = chupa_text_new());
    TAKE_STRING(sample_path = cut_build_fixture_data_path(fixture, NULL));
    TAKE_OBJECT(sample_file = g_file_new_for_path(sample_path));
    TAKE_OBJECT(text_input = chupa_text_input_new_from_file(NULL, sample_file));
    return TAKE_STRING(chupa_text_decompose_all(chupar, text_input));
}

void
test_decompose_pdf(void)
{
    cut_assert_equal_string("sample\n", decompose_pdf("sample.pdf"));
}
