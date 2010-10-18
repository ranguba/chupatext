/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-input.h>
#include <gsf/gsf-input-impl.h>
#include <chupatext/chupa_metadata.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_TEXT chupa_text_get_type()
#define CHUPA_TEXT(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT, ChupaText)
#define CHUPA_TEXT_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT, ChupaTextClass)
#define CHUPA_IS_TEXT(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT)
#define CHUPA_IS_TEXT_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT)
#define CHUPA_TEXT_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT, ChupaTextClass)

#define CHUPA_TEXT_SIGNAL_FINISHED chupa_text_signal_finished

typedef struct _ChupaText ChupaText;
typedef struct _ChupaTextClass ChupaTextClass;

struct _ChupaText
{
    GObject parent_instance;
};

struct _ChupaTextClass
{
    GObjectClass parent_class;

    /* signals */
    void (*finished)(GObject *object);
};

typedef void (*ChupaTextCallback)(ChupaText *, gpointer);

GType          chupa_text_get_type         (void) G_GNUC_CONST;
ChupaText     *chupa_text_new              (ChupaMetadata *metadata, GsfInput *inpt);
ChupaText     *chupa_text_new_from_stream  (ChupaMetadata *metadata,
                                            GInputStream *stream,
                                            const char *path);
ChupaText     *chupa_text_new_from_file    (ChupaMetadata *metadata, GFile *file, GError **err);
ChupaMetadata *chupa_text_get_metadata     (ChupaText *input);
const gchar   *chupa_text_get_mime_type    (ChupaText *input);
void           chupa_text_set_mime_type    (ChupaText *input, const char *mime_type);
const gchar   *chupa_text_get_filename     (ChupaText *input);
void           chupa_text_set_filename     (ChupaText *input, const char *filename);
const gchar   *chupa_text_get_charset      (ChupaText *input);
void           chupa_text_set_charset      (ChupaText *input, const char *charset);
GsfInput      *chupa_text_get_base_input   (ChupaText *input);
GInputStream  *chupa_text_get_stream       (ChupaText *input);
void           chupa_text_finished         (ChupaText *input);

G_END_DECLS

#endif  /* CHUPA_TEXT_H */
