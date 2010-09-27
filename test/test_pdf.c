/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; encoding: utf-8 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_test_util.h"

void test_decompose_pdf(void);
void test_decompose_pdf_multi_pages(void);

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

#define decompose_pdf chupa_test_decompose_fixture

void
test_decompose_pdf(void)
{
    cut_assert_equal_string("sample\n", decompose_pdf("sample.pdf", NULL));
}

void
test_decompose_pdf_multi_pages(void)
{
    cut_assert_equal_string("page1\n"
                            "2ページ目\n"
                            "page3\n",
                            decompose_pdf("sample_multi_pages.pdf", NULL));
}
