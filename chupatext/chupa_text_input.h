/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPA_TEXT_INPUT_H
#define CHUPA_TEXT_INPUT_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-input.h>
#include <gsf/gsf-input-impl.h>
#include "chupatext/chupa_metadata.h"

G_BEGIN_DECLS

#define CHUPA_TYPE_TEXT_INPUT chupa_text_input_get_type()
#define CHUPA_TEXT_INPUT(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT_INPUT, ChupaTextInput)
#define CHUPA_TEXT_INPUT_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT_INPUT, ChupaTextInputClass)
#define CHUPA_IS_TEXT_INPUT(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT_INPUT)
#define CHUPA_IS_TEXT_INPUT_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT_INPUT)
#define CHUPA_TEXT_INPUT_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT_INPUT, ChupaTextInputClass)

typedef struct _ChupaTextInput ChupaTextInput;
typedef struct _ChupaTextInputClass ChupaTextInputClass;

struct _ChupaTextInput
{
    GObject parent_instance;
};

struct _ChupaTextInputClass
{
    GObjectClass parent_class;
};

GType chupa_text_input_get_type(void) G_GNUC_CONST;
ChupaTextInput *chupa_text_input_new(ChupaMetadata *metadata, GsfInput *inpt);
ChupaTextInput *chupa_text_input_new_from_stream(ChupaMetadata *metadata, GInputStream *stream, const char *path);
ChupaTextInput *chupa_text_input_new_from_file(ChupaMetadata *metadata, GFile *file, GError **err);
ChupaMetadata *chupa_text_input_get_metadata(ChupaTextInput *input);
const gchar *chupa_text_input_get_mime_type(ChupaTextInput *input);
const gchar *chupa_text_input_get_filename(ChupaTextInput *input);
void chupa_text_input_set_filename(ChupaTextInput *input, const char *filename);
const gchar *chupa_text_input_get_charset(ChupaTextInput *input);
void chupa_text_input_set_charset(ChupaTextInput *input, const char *charset);
GsfInput *chupa_text_input_get_base_input(ChupaTextInput *input);
GInputStream *chupa_text_input_get_stream(ChupaTextInput *input);

G_END_DECLS

#endif  /* CHUPA_TEXT_INPUT_H */
