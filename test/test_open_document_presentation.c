/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

void test_1_2(void);
void test_1_2_ja(void);
void test_1_2_flat(void);
void test_1_2_ja_flat(void);

void
setup(void)
{
    chupa_test_setup("open_document_presentation");
}

void
teardown(void)
{
    chupa_test_teardown();
}

void
test_1_2(void)
{
    cut_assert_equal_string("         Sample\n"
                            "OpenDocument Presentation 1.2\n"
                            "\f"
                            "   Page\n"
                            " 2\n"
                            "\n",
                            decompose("1.2.odp"));
}

void
test_1_2_ja(void)
{
    cut_assert_equal_string("      Sample/サンプル\n"
                            "OpenDocument Presentation 1.2\n"
                            "\f"
                            "       Page\n"
                            " 2\n"
                            "\n"
                            " 2ページ目\n"
                            "\n",
                            decompose("1.2_ja.odp"));
}

void
test_1_2_flat(void)
{
    cut_assert_equal_string("           Sample\n"
                            "OpenDocument Presentation 1.2 (flat)\n"
                            "\f"
                            "   Page\n"
                            " 2\n"
                            "\n",
                            decompose("1.2.fodp"));
}

void
test_1_2_ja_flat(void)
{
    cut_assert_equal_string("       Sample/ サンプル\n"
                            "OpenDocument Presentation 1.2 (flat)\n"
                            "\f"
                            "        Page\n"
                            " 2\n"
                            "\n"
                            " 2 ページ目\n"
                            "\n",
                            decompose("1.2_ja.fodp"));
}
