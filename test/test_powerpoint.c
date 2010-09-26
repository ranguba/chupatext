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

#define decompose_msppt chupa_test_decompose_fixture

void
test_decompose_ppt(void)
{
    cut_assert_equal_string("Sample Title\n\n", decompose_msppt("sample.ppt", NULL));
}

void
test_decompose_ppt_ja(void)
{
}
