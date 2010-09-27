/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_test_util.h"

void test_decompose_msword(void);
void test_decompose_msword_ja(void);
void test_decompose_msword_ja97(void);
void test_decompose_msword_ja95(void);

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

void
test_decompose_msword_ja97(void)
{
    cut_assert_equal_string("Sample of Word97\n日本語サンプルWord97\n", decompose_msword("ja97.doc", NULL));
}

void
test_decompose_msword_ja95(void)
{
    cut_assert_equal_string("Sample of Word95\n日本語サンプルWord95\n", decompose_msword("ja95.doc", NULL));
}
