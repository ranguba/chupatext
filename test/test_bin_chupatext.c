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

#include <gcutter.h>
#include "chupa_test_util.h"

void test_html (void);

static GCutProcess *chupatext_process;
static GString *output_from_chupatext;

void
setup (void)
{
    chupatext_process = NULL;
    chupa_test_setup();
}

void
teardown (void)
{
    chupa_test_teardown();
    if (chupatext_process)
        g_object_unref(chupatext_process);
    if (output_from_chupatext)
        g_string_free(output_from_chupatext, TRUE);
}

static GCutProcess *
start(GCutProcess *process)
{
    chupatext_process = process;
    gcut_process_set_forced_termination_wait_time(process, 8000);
    if (!gcut_process_run(process, NULL))
        return NULL;
    gcut_process_wait(process, 6000, NULL);
    return process;
}

#define CHUPATEXT_COMMAND "./chupatext/chupatext"

#define TAKE_STRING(str) cut_take_string(str)
#define FIXTURE(fixture) TAKE_STRING(cut_build_fixture_data_path(fixture, NULL))
#define output_string(process) gcut_process_get_output_string(start(process))

void
test_html(void)
{
    const gchar *path = FIXTURE("sample.html");
    GCutProcess *process = gcut_process_new(CHUPATEXT_COMMAND, path, NULL);
    GString *result = output_string(process);
    cut_assert_equal_string("URL: sample.html\n"
                            "filename: sample.html\n"
                            "title: Sample HTML File\n"
                            "mime-type: text/plain\n"
                            "output-length: 17\n"
                            "encoding: UTF-8\n"
                            "\n"
                            "This is a sample.\n\n",
                            result->str);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
