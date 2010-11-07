/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

#include "chupa_ruby.h"

void
chupa_ruby_exception_inspect(VALUE exception, GString *buffer)
{
    VALUE message, class_name, backtrace;
    long i;

    message = rb_funcall(exception, rb_intern("message"), 0);
    class_name = rb_funcall(CLASS_OF(exception), rb_intern("to_s"), 0);
    g_string_append_printf(buffer, "%s (%s)\n",
                           RVAL2CSTR(message),
                           RVAL2CSTR(class_name));
    backtrace = rb_funcall(exception, rb_intern("backtrace"), 0);
    for (i = 0; i < RARRAY_LEN(backtrace); i++) {
        VALUE line;
        line = RARRAY_PTR(backtrace)[i];
        g_string_append_printf(buffer, "%s\n", RVAL2CSTR(line));
    }
}

void
chupa_ruby_exception_to_g_error(VALUE exception, GError **error,
                                GQuark domain, gint code,
                                const gchar *format, ...)
{
    GString *inspected;
    va_list args;

    if (NIL_P(exception))
        return;
    if (!error)
        return;

    inspected = g_string_new(NULL);
    va_start(args, format);
    g_string_vprintf(inspected, format, args);
    va_end(args);
    g_string_append(inspected, ": <");
    chupa_ruby_exception_inspect(exception, inspected);
    g_string_append(inspected, ">");
    g_set_error_literal(error, domain, code, inspected->str);
    g_string_free(inspected, TRUE);
}
