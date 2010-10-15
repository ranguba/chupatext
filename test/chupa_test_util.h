/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#ifndef CHUPA_TEST_UTIL_H
#define CHUPA_TEST_UTIL_H

#include <chupatext/chupa_feeder.h>
#include <gio/gio.h>

#include <gcutter.h>
#include <cutter/cut-run-context.h>

void chupa_test_setup(void);
void chupa_test_teardown(void);
const gchar *chupa_test_decompose_fixture(const char *fixture, GError **error);
const gchar *chupa_test_decompose_data(const char *text, gsize size, GError **error);
ChupaTextInput *chupa_test_decomposer_from_data(const char *text, gsize size, GError **error);
ChupaTextInput *chupa_test_decomposer_from_fixture(const char *fixture, GError **error);
const gchar *chupa_test_decompose_all(ChupaTextInput *text_input, GError **error);
ChupaMetadata *chupa_test_decompose_metadata(ChupaTextInput *text_input, GError **error);
ChupaMetadata *chupa_test_metadata_fixture(const char *fixture, GError **error);

#endif
