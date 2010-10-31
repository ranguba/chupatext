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

static const gchar *temp_dir;
static const gchar *old_chupa_home;

void test_decompose_ppt(void);
void test_decompose_ppt_ja(void);

void
setup(void)
{
    const gchar *chupa_home;

    chupa_test_setup();
    temp_dir = cut_build_path(cut_get_test_directory(), "temp", NULL);
    chupa_home = cut_build_path(temp_dir, "chupa_home", NULL);
    old_chupa_home = g_getenv("CHUPA_HOME");
    g_setenv("CHUPA_HOME", chupa_home, TRUE);
    cut_make_directory(chupa_home,
                       NULL);
}

void
teardown(void)
{
    cut_remove_path(temp_dir, NULL);
    g_setenv("CHUPA_HOME", old_chupa_home, TRUE);
    chupa_test_teardown();
}

#define decompose_msppt chupa_test_decompose_fixture

void
test_decompose_ppt(void)
{
    cut_assert_equal_string("Sample Title\n",
                            decompose_msppt("sample.ppt"));
}

void
test_decompose_ppt_ja(void)
{
    cut_assert_equal_string("Sample Title\nこんにちゅぱ\n",
                            decompose_msppt("sample_ja.ppt"));
}
