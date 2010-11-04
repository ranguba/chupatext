/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#ifndef CHUPA_METADATA_H
#define CHUPA_METADATA_H

#include <glib.h>
#include <glib-object.h>

#include <chupatext/chupa_types.h>

G_BEGIN_DECLS

typedef struct _ChupaMetadataField ChupaMetadataField;
typedef struct _ChupaMetadataFieldClass ChupaMetadataFieldClass;

typedef void   (*ChupaMetadataFieldCallback) (ChupaMetadataField *field,
                                              gpointer            user_data);

#define CHUPA_METADATA_ERROR           (chupa_metadata_error_quark())

#define CHUPA_TYPE_METADATA            chupa_metadata_get_type()
#define CHUPA_METADATA(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_METADATA, ChupaMetadata)
#define CHUPA_METADATA_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_METADATA, ChupaMetadataClass)
#define CHUPA_IS_METADATA(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_METADATA)
#define CHUPA_IS_METADATA_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_METADATA)
#define CHUPA_METADATA_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_METADATA, ChupaMetadataClass)

typedef struct _ChupaMetadata ChupaMetadata;
typedef struct _ChupaMetadataClass ChupaMetadataClass;

struct _ChupaMetadata
{
    GObject object;
};

struct _ChupaMetadataClass
{
    GObjectClass parent_class;
};

typedef enum
{
    CHUPA_METADATA_ERROR_NOT_EXIST
} ChupaMetadataError;

GQuark         chupa_metadata_error_quark     (void);

GType          chupa_metadata_get_type        (void) G_GNUC_CONST;
ChupaMetadata *chupa_metadata_new             (void);
void           chupa_metadata_update          (ChupaMetadata *metadata,
                                               ChupaMetadata *update);
guint          chupa_metadata_size            (ChupaMetadata *metadata);
void           chupa_metadata_foreach         (ChupaMetadata *metadata,
                                               ChupaMetadataFieldCallback callback,
                                               gpointer       user_data);
gboolean       chupa_metadata_remove          (ChupaMetadata *metadata,
                                               const gchar   *key);
void           chupa_metadata_set_string      (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               const gchar   *value);
const gchar   *chupa_metadata_get_string      (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               GError       **error);
void           chupa_metadata_set_int         (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               gint           value);
gint           chupa_metadata_get_int         (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               GError       **error);
void           chupa_metadata_set_time_val    (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               GTimeVal      *time_val);
GTimeVal      *chupa_metadata_get_time_val    (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               GError       **error);
void           chupa_metadata_set_size        (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               gsize          size);
gsize          chupa_metadata_get_size        (ChupaMetadata *metadata,
                                               const gchar   *key,
                                               GError       **error);

void           chupa_metadata_set_content_length
                                              (ChupaMetadata *metadata,
                                               gsize          length);
gsize          chupa_metadata_get_content_length
                                              (ChupaMetadata *metadata);

#define CHUPA_TYPE_METADATA_FIELD            chupa_metadata_field_get_type()
#define CHUPA_METADATA_FIELD(obj)                               \
    G_TYPE_CHECK_INSTANCE_CAST(obj,                             \
                               CHUPA_TYPE_METADATA_FIELD,       \
                               ChupaMetadataField)
#define CHUPA_METADATA_FIELD_CLASS(klass)               \
    G_TYPE_CHECK_CLASS_CAST(klass,                      \
                            CHUPA_TYPE_METADATA_FIELD,  \
                            ChupaMetadataFieldClass)
#define CHUPA_IS_METADATA_FIELD(obj)                            \
    G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_METADATA_FIELD)
#define CHUPA_IS_METADATA_FIELD_CLASS(klass)                    \
    G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_METADATA_FIELD)
#define CHUPA_METADATA_FIELD_GET_CLASS(obj)                             \
    G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_METADATA_FIELD,           \
                              ChupaMetadataFieldClass)

struct _ChupaMetadataField
{
    GObject object;
};

struct _ChupaMetadataFieldClass
{
    GObjectClass parent_class;
};

GType          chupa_metadata_field_get_type        (void) G_GNUC_CONST;
const gchar   *chupa_metadata_field_name            (ChupaMetadataField *field);
GType          chupa_metadata_field_type            (ChupaMetadataField *field);
const gchar   *chupa_metadata_field_value_string    (ChupaMetadataField *field);
gint           chupa_metadata_field_value_int       (ChupaMetadataField *field);
GTimeVal      *chupa_metadata_field_value_time_val  (ChupaMetadataField *field);
gsize          chupa_metadata_field_value_size      (ChupaMetadataField *field);
const gchar   *chupa_metadata_field_to_string       (ChupaMetadataField *field);

G_END_DECLS

#endif  /* CHUPA_METADATA_H */
