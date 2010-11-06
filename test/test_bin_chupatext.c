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

void test_html(void);
void test_excel(void);
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
run(GCutProcess *process)
{
    GError *error = NULL;

    gcut_process_set_forced_termination_wait_time(process, 8000);
    gcut_process_run(process, &error);
    gcut_assert_error(error,
                      cut_message("error message: <%s>",
                                  gcut_process_get_error_string(process)->str));
    gcut_process_wait(process, 6000, &error);
    gcut_assert_error(error,
                      cut_message("error message: <%s>",
                                  gcut_process_get_error_string(process)->str));
    return gcut_process_get_output_string(process)->str;
}

#define CHUPATEXT_COMMAND "./chupatext/chupatext"

#define TAKE_STRING(str) cut_take_string(str)
#define FIXTURE(fixture) TAKE_STRING(cut_build_fixture_data_path(fixture, NULL))

void
test_html(void)
{
    const gchar *path = FIXTURE("sample.html");

    chupatext = gcut_process_new(CHUPATEXT_COMMAND, path, NULL);
    cut_assert_equal_string("URI: sample.html\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 120\n"
                            "Content-Length: 17\n"
                            "Title: Sample HTML File\n"
                            "Original-Filename: sample.html\n"
                            "Original-Content-Type: text/html\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample.html;"
                            " size=120\n"
                            "\n"
                            "This is a sample.",
                            run(chupatext));
}

void
test_excel(void)
{
    const gchar *path = FIXTURE("sample.xls");

    chupatext = gcut_process_new(CHUPATEXT_COMMAND, path, NULL);
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
                            "sample\n1\n2\n3\n4\n5\n6\n7\n",
                            run(chupatext));
}

void
test_pdf_multi_pages(void)
{
    const gchar *path = FIXTURE("sample_multi_pages.pdf");

    chupatext = gcut_process_new(CHUPATEXT_COMMAND, path, NULL);
    cut_assert_equal_string("URI: sample_multi_pages.pdf\n"
                            "Content-Type: text/plain; charset=UTF-8\n"
                            "Original-Content-Length: 6145\n"
                            "Creation-Time: 2010-09-27T04:09:17Z\n"
                            "Original-Filename: sample_multi_pages.pdf\n"
                            "Original-Content-Type: application/pdf\n"
                            "Original-Content-Disposition: inline;"
                            " filename=sample_multi_pages.pdf;"
                            " size=6145;"
                            " creation-date=2010-09-27T04:09:17Z\n"
                            "\n"
                            "page1\n\f"
                            "2 ページ目\n\f"
                            "page3",
                            run(chupatext));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
