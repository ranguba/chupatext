/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPA_TEST_UTIL_H
#define CHUPA_TEST_UTIL_H

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>
#include <cutter/cut-run-context.h>

void chupa_test_setup(void);
void chupa_test_teardown(void);
char *chupa_test_decompose_fixture(const char *fixture, GError **err);
char *chupa_test_decompose_data(const char *text, gsize size, GError **err);

#endif
