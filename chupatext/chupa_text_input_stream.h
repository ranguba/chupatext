/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPA_TEXT_INPUT_STREAM_H
#define CHUPA_TEXT_INPUT_STREAM_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "chupatext/chupa_metadata.h"

G_BEGIN_DECLS

#define CHUPA_TYPE_TEXT_INPUT_STREAM            chupa_text_input_stream_get_type()
#define CHUPA_TEXT_INPUT_STREAM(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT_INPUT_STREAM, ChupaTextInputStream)
#define CHUPA_TEXT_INPUT_STREAM_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT_INPUT_STREAM, ChupaTextInputStreamClass)
#define CHUPA_IS_TEXT_INPUT_STREAM(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT_INPUT_STREAM)
#define CHUPA_IS_TEXT_INPUT_STREAM_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT_INPUT_STREAM)
#define CHUPA_TEXT_INPUT_STREAM_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT_INPUT_STREAM, ChupaTextInputStreamClass)

typedef struct _ChupaTextInputStream ChupaTextInputStream;
typedef struct _ChupaTextInputStreamClass ChupaTextInputStreamClass;

struct _ChupaTextInputStream
{
    GDataInputStream object;
};

struct _ChupaTextInputStreamClass
{
    GDataInputStreamClass parent_class;
};

GType        chupa_text_input_stream_get_type(void) G_GNUC_CONST;
ChupaTextInputStream *chupa_text_input_stream_new(ChupaMetadata *metadata, GInputStream *input);
ChupaMetadata *chupa_text_input_stream_get_metadata(ChupaTextInputStream *stream);
const gchar *chupa_text_input_stream_get_mime_type(ChupaTextInputStream *stream);

G_END_DECLS

#endif  /* CHUPA_TEXT_INPUT_STREAM_H */
