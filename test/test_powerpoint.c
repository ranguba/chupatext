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

#include "chupa_test_util.h"

void test_xp(void);
void test_xp_ja(void);
void test_2007(void);
void test_2007_ja(void);

void
setup(void)
{
    chupa_test_setup("powerpoint");
}

void
teardown(void)
{
    chupa_test_teardown();
}

void
test_xp(void)
{
    cut_assert_equal_string("  Sample\n"
                            "PowerPoint XP\n"
                            "\f"
                            "    Page\n"
                            "  2\n"
                            "\n",
                            decompose("xp.ppt"));
}

void
test_xp_ja(void)
{
    cut_assert_equal_string("Sample/サンプル\n"
                            "  PowerPoint XP\n"
                            "\f"
                            "        Page\n"
                            "  2\n"
                            "\n"
                            "  2ページ目\n"
                            "\n",
                            decompose("xp_ja.ppt"));
}

void
test_2007(void)
{
    cut_assert_equal_string("   Sample\n"
                            "PowerPoint 2007\n"
                            "\f"
                            "   Page\n"
                            " 2\n"
                            "\n",
                            decompose("2007.pptx"));
}

void
test_2007_ja(void)
{
    cut_assert_equal_string("Sample/サンプル\n"
                            "PowerPoint 2007\n"
                            "\f"
                            "       Page\n"
                            " 2\n"
                            "\n"
                            " 2ページ目\n"
                            "\n",
                            decompose("2007_ja.pptx"));
}
