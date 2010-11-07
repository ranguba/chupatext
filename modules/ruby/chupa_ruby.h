/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef CHUPA_RUBY_H
#define CHUPA_RUBY_H

#include <chupatext.h>

#include <ruby.h>
#include <ruby/encoding.h>

#include <rbgobject.h>

#ifdef RUBY_API_VERSION_CODE
#define RUBY_CHECK_VERSION(major, minor, teeny) \
    (major * 10000 + minor * 100 + teeny) >= RUBY_API_VERSION_CODE
#else
#define RUBY_CHECK_VERSION(major, minor, teeny)	0
#endif

void     chupa_ruby_init                       (void);
VALUE    chupa_ruby_g_memory_input_stream_init (VALUE mGLib);
VALUE    chupa_ruby_feeder_init                (VALUE mChupa);
VALUE    chupa_ruby_data_init                  (VALUE mChupa);
VALUE    chupa_ruby_decomposer_init            (VALUE mChupa);
VALUE    chupa_ruby_metadata_init              (VALUE mChupa);

VALUE    chupa_ruby_metadata_new               (ChupaMetadata *metadata,
                                                gboolean       readonly);

GType    chupa_ruby_decomposer_get_type        (void);

void     chupa_ruby_exception_inspect          (VALUE          exception,
                                                GString       *buffer);
void     chupa_ruby_exception_to_g_error       (VALUE          exception,
                                                GError       **error,
                                                GQuark         domain,
                                                gint           code,
                                                const gchar   *format,
                                                ...) G_GNUC_PRINTF(5, 6);

#endif
