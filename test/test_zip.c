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

#define decompose_zip chupa_test_decompose_fixture

void
test_decompose_zip(void)
{
    cut_assert_equal_string("sample\n", decompose_zip("sample.zip"));
}
