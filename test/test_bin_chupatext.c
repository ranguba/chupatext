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

#include <gcutter.h>
#include "chupa_test_util.h"

void test_text(void);
void test_html(void);
void test_word(void);
void test_excel(void);
void test_powerpoint(void);
void test_pdf_multi_pages(void);

static GCutProcess *chupatext;

void
setup (void)
{
    chupatext = NULL;
    chupa_test_setup();
}

void
teardown (void)
{
    chupa_test_teardown();
    if (chupatext)
        g_object_unref(chupatext);
}

static const gchar *
run_process(GCutProcess *process)
{
    GError *error = NULL;
    gint status;
    GString *error_string;

    error_string = gcut_process_get_error_string(process);

    gcut_process_set_forced_termination_wait_time(process, 8000);
    gcut_process_run(process, &error);
    gcut_assert_error(error,
                      cut_message("error message: <%s>",
                                  error_string->str));
    status = gcut_process_wait(process, 6000, &error);
    gcut_assert_error(error,
                      cut_message("error message: <%s>",
                                  error_string->str));
    cut_assert_equal_int(0, status,
                         cut_message("chupatext exists abnormally: %d: <%s>",
                                     status,
                                     error_string->str));
    return gcut_process_get_output_string(process)->str;
}

#define CHUPATEXT_COMMAND "./chupatext/chupatext"

static const gchar *
run(const gchar *fixture_file)
{
    const gchar *full_path;

    full_path = cut_take_string(cut_build_fixture_data_path(fixture_file, NULL));
    chupatext = gcut_process_new(CHUPATEXT_COMMAND, full_path, NULL);
    return run_process(chupatext);
}

void
test_text(void)
{
    cut_assert_equal_string("URI: sample.txt\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 7\n"
                            "Content-Length: 7\n"
                            "Original-Filename: sample.txt\n"
                            "Original-Content-Type: text/plain\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample.txt;"
                            " size=7\n"
                            "\n"
                            "sample\n"
                            "\n",
                            run("sample.txt"));
}

void
test_html(void)
{
    cut_assert_equal_string("URI: sample.html\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 120\n"
                            "Title: Sample HTML File\n"
                            "Content-Length: 17\n"
                            "Original-Filename: sample.html\n"
                            "Original-Content-Type: text/html\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample.html;"
                            " size=120\n"
                            "\n"
                            "This is a sample."
                            "\n",
                            run("sample.html"));
}

void
test_word(void)
{
    cut_assert_equal_string("URI: sample.doc\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 9216\n"
                            "Content-Length: 8\n"
                            "Original-Filename: sample.doc\n"
                            "Original-Content-Type: application/msword\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample.doc;"
                            " size=9216\n"
                            "\n"
                            "Sample\n\n"
                            "\n",
                            run("sample.doc"));
}

void
test_excel(void)
{
    cut_assert_equal_string("URI: sample.xls\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 5632\n"
                            "Content-Length: 21\n"
                            "Original-Filename: sample.xls\n"
                            "Original-Content-Type: application/vnd.ms-excel\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample.xls;"
                            " size=5632\n"
                            "\n"
                            "sample\n1\n2\n3\n4\n5\n6\n7\n"
                            "\n",
                            run("sample.xls"));
}

void
test_powerpoint(void)
{
    cut_assert_equal_string("URI: sample.ppt\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 72704\n"
                            "Author: Nobuyoshi Nakada\n"
                            "Content-Length: 13\n"
                            "Original-Filename: sample.ppt\n"
                            "Original-Content-Type: application/vnd.ms-powerpoint\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample.ppt;"
                            " size=72704\n"
                            "\n"
                            "Sample Title\n"
                            "\n",
                            run("sample.ppt"));
}

void
test_pdf_multi_pages(void)
{
    cut_assert_equal_string("URI: sample_multi_pages.pdf\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 6145\n"
                            "Creation-Time: 2010-09-27T04:09:17Z\n"
                            "Content-Length: 29\n"
                            "Original-Filename: sample_multi_pages.pdf\n"
                            "Original-Content-Type: application/pdf\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample_multi_pages.pdf;"
                            " size=6145;"
                            " creation-date=2010-09-27T04:09:17Z\n"
                            "\n"
                            "page1\n\f"
                            "2 ページ目\n\f"
                            "page3\n"
                            "\n",
                            run("sample_multi_pages.pdf"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
