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

#ifndef CHUPA_TEXT_H
#define CHUPA_TEXT_H

#include <gio/gio.h>
#include <glib-object.h>
#include "chupatext/chupa_text_input.h"

G_BEGIN_DECLS

#define CHUPA_TYPE_TEXT            chupa_text_get_type()
#define CHUPA_TEXT(obj)            G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT, ChupaText)
#define CHUPA_TEXT_CLASS(klass)    G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT, ChupaTextClass)
#define IS_CHUPA_TEXT(obj)         G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT)
#define IS_CHUPA_TEXT_CLASS(klass) G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT)
#define CHUPA_TEXT_GET_CLASS(obj)  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT, ChupaTextClass)

#define CHUPA_TEXT_SIGNAL_DECOMPOSED chupa_text_signal_decomposed
extern const char chupa_text_signal_decomposed[];

typedef struct ChupaText ChupaText;
typedef struct ChupaTextClass ChupaTextClass;

/**
 * ChupaText:
 *
 * The class to decompose an input and extract text portions. 
 */
struct ChupaText
{
    GObject parent_instance;
};

/**
 * ChupaTextClass:
 *
 * @decomposed: the callback function called when each text portion is
 * found.
 */
struct ChupaTextClass
{
    GObjectClass parent_class;

    /* signals */
    void (*decomposed)(GObject *object, ChupaTextInput *input);
};

/**
 * ChupaTextCallback:
 *
 * The type used for callback functions when extracting the text portion.
 */
typedef void (*ChupaTextCallback)(ChupaText *, ChupaTextInput *, gpointer);

GType chupa_text_get_type(void) G_GNUC_CONST;
ChupaText *chupa_text_new(void);
gboolean chupa_text_feed(ChupaText *chupar, ChupaTextInput *input, GError **err);
void chupa_text_decomposed(ChupaText *chupar, ChupaTextInput *input);
void chupa_text_decompose(ChupaText *chupar, ChupaTextInput *text_input,
                          ChupaTextCallback func, gpointer arg, GError **error);
char *chupa_text_decompose_all(ChupaText *chupar, ChupaTextInput *text_input, GError **error);

typedef enum {
    CHUPA_TEXT_ERROR_NONE,
    CHUPA_TEXT_ERROR_UNKNOWN_CONTENT,
    CHUPA_TEXT_ERROR_UNKNOWN_MIMETYPE,
    CHUPA_TEXT_ERROR_INVALID_INPUT,
    CHUPA_TEXT_ERROR_MAX_
} ChupaTextError;

#define CHUPA_TEXT_ERROR chupa_text_error_quark()

GQuark chupa_text_error_quark(void) G_GNUC_CONST;
GError *chupa_text_error_new_valist(ChupaTextError code, const char *format, va_list args);
GError *chupa_text_error_new(ChupaTextError code, const char *format, ...);
GError *chupa_text_error_new_literal(ChupaTextError code, const char *message);

#endif
