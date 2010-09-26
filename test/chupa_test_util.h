/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPA_TEST_UTIL_H
#define CHUPA_TEST_UTIL_H

#include <chupatext/chupa_text.h>
#include <gio/gio.h>

#include <gcutter.h>
#include <cutter/cut-run-context.h>

void chupa_test_setup(void);
void chupa_test_teardown(void);
char *chupa_test_decompose_fixture(const char *fixture, GError **err);
char *chupa_test_decompose_data(const char *text, gsize size, GError **err);
ChupaTextInput *chupa_test_decomposer_from_data(const char *text, gsize size, GError **err);
ChupaTextInput *chupa_test_decomposer_from_fixture(const char *fixture, GError **err);
char *chupa_test_decompose_all(ChupaTextInput *text_input, GError **err);

#endif
