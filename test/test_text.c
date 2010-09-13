/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <chupatext/chupa_text_input.h>
#include "chupa_test_util.h"
#include <gio/gio.h>

#include <gcutter.h>

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

#define decompose_text chupa_test_decompose_data

void
test_decompose_text_plain (void)
{
    static const char plain_text[] = "plain text\n";

    cut_assert_equal_string(plain_text, decompose_text(plain_text, sizeof(plain_text) - 1, NULL));
}
