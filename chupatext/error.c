/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_text.h"


GQuark
chupa_text_error_quark(void)
{
    static GQuark error_quark = 0;

    if (G_UNLIKELY(error_quark == 0)) {
        error_quark = g_quark_from_static_string("chupa-text-error-quark");
    }

    return error_quark;
}

GError *
chupa_text_error_new_valist(ChupaTextError code, const char *format, va_list args)
{
    return g_error_new_valist(CHUPA_TEXT_ERROR, code, format, args);
}

GError *
chupa_text_error_new(ChupaTextError code, const char *format, ...)
{
    va_list args;
    GError *error;

    va_start(args, format);
    error = chupa_text_error_new_valist(code, format, args);
    va_end(args);
    return error;
}

GError *
chupa_text_error_new_literal(ChupaTextError code, const char *message)
{
    return g_error_new_literal(CHUPA_TEXT_ERROR, code, message);
}