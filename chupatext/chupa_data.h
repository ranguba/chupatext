/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *  Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

#ifndef CHUPA_DATA_H
#define CHUPA_DATA_H

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <chupatext/chupa_metadata.h>
#include <chupatext/chupa_utils.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_DATA chupa_data_get_type()
#define CHUPA_DATA(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_DATA, ChupaData)
#define CHUPA_DATA_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_DATA, ChupaDataClass)
#define CHUPA_IS_DATA(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_DATA)
#define CHUPA_IS_DATA_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_DATA)
#define CHUPA_DATA_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_DATA, ChupaDataClass)

#define CHUPA_DATA_SIGNAL_FINISHED chupa_data_signal_finished

typedef struct _ChupaData ChupaData;
typedef struct _ChupaDataClass ChupaDataClass;

struct _ChupaData
{
    GObject parent_instance;
};

struct _ChupaDataClass
{
    GObjectClass parent_class;

    /* signals */
    void (*finished)(ChupaData *data,
                     GError    *error);
};

GType          chupa_data_get_type         (void) G_GNUC_CONST;
ChupaData     *chupa_data_new              (GInputStream  *stream,
                                            ChupaMetadata *metadata);
ChupaData     *chupa_data_new_from_file    (GFile         *file,
                                            ChupaMetadata *metadata,
                                            GError       **error);
ChupaMetadata *chupa_data_get_metadata     (ChupaData *data);
const gchar   *chupa_data_get_mime_type    (ChupaData *data);
void           chupa_data_set_mime_type    (ChupaData *data, const char *mime_type);
const gchar   *chupa_data_get_filename     (ChupaData *data);
void           chupa_data_set_filename     (ChupaData *data, const char *filename);
const gchar   *chupa_data_get_charset      (ChupaData *data);
void           chupa_data_set_charset      (ChupaData *data, const char *charset);
GInputStream  *chupa_data_get_stream       (ChupaData *data);
void           chupa_data_finished         (ChupaData *data,
                                            GError    *error);

gboolean       chupa_data_is_text          (ChupaData *data);
gboolean       chupa_data_is_succeeded     (ChupaData *data);
gboolean       chupa_data_is_finished      (ChupaData *data);
GError        *chupa_data_get_error        (ChupaData *data);
const gchar   *chupa_data_get_raw_data     (ChupaData *data,
                                            gsize     *length,
                                            GError   **error);

G_END_DECLS

#endif  /* CHUPA_DATA_H */
