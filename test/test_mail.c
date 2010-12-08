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

void test_decompose_mail(void);
void test_decompose_mail_subject(void);

void
setup(void)
{
    chupa_test_setup();
}

void
teardown(void)
{
    chupa_test_teardown();
}

#define decompose_fixture chupa_test_decompose_fixture

void
test_decompose_mail(void)
{
    cut_assert_equal_string("This is a test mail.",
                            decompose_fixture("sample.eml"));
}

void
test_decompose_mail_subject(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("sample.eml");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("test mail",
                            chupa_metadata_get_string(metadata, "title", NULL));
}
