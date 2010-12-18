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
    if (format) {
        va_start(args, format);
        g_string_vprintf(inspected, format, args);
        va_end(args);
        g_string_append(inspected, ": ");
    }
    g_string_append(inspected, "<");
    chupa_ruby_exception_inspect(exception, inspected);
    g_string_append(inspected, ">");
    g_set_error_literal(error, domain, code, inspected->str);
    g_string_free(inspected, TRUE);
}

gboolean
chupa_ruby_exception_to_g_error_info(VALUE exception, GQuark *domain, gint *code,
                                     gchar **code_name)
{
    ID id_at_code, id_at_domain, id_to_i;
    VALUE rb_error_code = Qnil, rb_error_domain = Qnil;

    if (NIL_P(exception))
        return FALSE;

    CONST_ID(id_at_code, "@code");
    CONST_ID(id_at_domain, "@domain");

    if (!RTEST(rb_ivar_defined(exception, id_at_code)))
        return FALSE;

    if (!RTEST(rb_ivar_defined(exception, id_at_domain)))
        return FALSE;

    rb_error_code = rb_ivar_get(exception, id_at_code);
    rb_error_domain = rb_ivar_get(exception, id_at_domain);

    if (NIL_P(rb_error_code) || NIL_P(rb_error_domain))
        return FALSE;

    *domain = g_quark_from_string(StringValueCStr(rb_error_domain));

    if (code_name) {
        ID id_nick, id_to_s;
        VALUE rb_code_name;

        CONST_ID(id_nick, "nick");
        CONST_ID(id_to_s, "to_s");
        if (rb_respond_to(rb_error_code, id_nick)) {
            rb_code_name = rb_funcall(rb_error_code, id_nick, 0);
        } else {
            rb_code_name = rb_funcall(rb_error_code, id_to_s, 0);
        }
        *code_name = g_strdup(StringValueCStr(rb_code_name));
    }

    CONST_ID(id_to_i, "to_i");
    rb_error_code = rb_funcall(rb_error_code, id_to_i, 0);
    *code = NUM2INT(rb_error_code);

    return TRUE;
}
