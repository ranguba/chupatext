/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

struct ChupaText
{
    GObject parent_instance;
};

struct ChupaTextClass
{
    GObjectClass parent_class;

    /* signals */
    void (*decomposed)(GObject *object, GInputStream *stream);
};

GType chupa_text_get_type(void) G_GNUC_CONST;
ChupaText *chupa_text_new(void);
void chupa_text_feed(ChupaText *chupar, ChupaTextInput *input);
void chupa_text_decomposed(ChupaText *chupar, ChupaTextInput *input);
guint chupa_text_connect_decomposed(ChupaText *chupar, GCallback *func, gpointer arg);

#endif
