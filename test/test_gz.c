/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

void test_decompose_tar_gz(void);
void test_decompose_txt_gz(void);

void
setup(void)
{
    chupa_test_setup(NULL);
}

void
teardown(void)
{
    chupa_test_teardown();
}

void
test_decompose_tar_gz(void)
{
    cut_assert_equal_string("sample\n", decompose("sample.tar.gz"));
}

void
test_decompose_txt_gz(void)
{
    cut_assert_equal_string("sample\n", decompose("sample.txt.gz"));
}
