/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_test_util.h"

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

#define decompose_msword chupa_test_decompose_fixture

void
test_decompose_msword(void)
{
    cut_assert_equal_string("Sample\n\n", decompose_msword("sample.doc", NULL));
}

void
test_decompose_msword_ja(void)
{
    cut_assert_equal_string("Sample\nあいうえお\n", decompose_msword("sample_ja.doc", NULL));
}
