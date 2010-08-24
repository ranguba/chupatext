/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPA_RESTRICT_INPUT_STREAM_H
#define CHUPA_RESTRICT_INPUT_STREAM_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_RESTRICT_INPUT_STREAM \
    chupa_restrict_input_stream_get_type()
#define CHUPA_RESTRICT_INPUT_STREAM(obj) \
    G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_RESTRICT_INPUT_STREAM, ChupaRestrictInputStream)
#define CHUPA_RESTRICT_INPUT_STREAM_CLASS(klass) \
    G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_RESTRICT_INPUT_STREAM, ChupaRestrictInputStreamClass)
#define CHUPA_IS_RESTRICT_INPUT_STREAM(obj) \
    G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_RESTRICT_INPUT_STREAM)
#define CHUPA_IS_RESTRICT_INPUT_STREAM_CLASS(klass) \
    G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_RESTRICT_INPUT_STREAM)
#define CHUPA_RESTRICT_INPUT_STREAM_GET_CLASS(obj) \
    G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_RESTRICT_INPUT_STREAM, ChupaRestrictInputStreamClass)

typedef struct _ChupaRestrictInputStream ChupaRestrictInputStream;
typedef struct _ChupaRestrictInputStreamClass ChupaRestrictInputStreamClass;

struct _ChupaRestrictInputStream
{
    GFilterInputStream object;
};

struct _ChupaRestrictInputStreamClass
{
    GFilterInputStreamClass parent_class;
};

GType        chupa_restrict_input_stream_get_type(void) G_GNUC_CONST;
GInputStream *chupa_restrict_input_stream_new(GInputStream *input, goffset length);
gsize chupa_restrict_input_stream_skip_to_end(GInputStream *input);

G_END_DECLS

#endif  /* CHUPA_RESTRICT_INPUT_STREAM_H */
