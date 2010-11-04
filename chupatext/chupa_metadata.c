/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <string.h>

#include "chupa_metadata.h"

#define CHUPA_METADATA_FIELD_GET_PRIVATE(obj)                   \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                         \
                                 CHUPA_TYPE_METADATA_FIELD,     \
                                 ChupaMetadataFieldPrivate))

typedef struct _ChupaMetadataFieldPrivate  ChupaMetadataFieldPrivate;
typedef struct _ChupaMetadataFieldPrivate
{
    ChupaMetadata *metadata;
    gchar *name;
    GType type;
    union {
        gint     integer;
        gsize    size;
        gpointer pointer;
    } value;
    GDestroyNotify free_function;
    GString *string;
    GString *value_string;
} Field;

G_DEFINE_TYPE(ChupaMetadataField, chupa_metadata_field, G_TYPE_OBJECT);

static void field_dispose        (GObject         *object);

static void
chupa_metadata_field_class_init (ChupaMetadataFieldClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = field_dispose;

    g_type_class_add_private(gobject_class, sizeof(ChupaMetadataFieldPrivate));
}

static void
chupa_metadata_field_init (ChupaMetadataField *field)
{
}

static void
field_dispose (GObject *object)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(object);

    if (priv->metadata) {
        g_object_unref(priv->metadata);
        priv->metadata = NULL;
    }

    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    if (priv->value.pointer) {
        if (priv->free_function) {
            priv->free_function(priv->value.pointer);
        }
    }

    if (priv->string) {
        g_string_free(priv->string, TRUE);
        priv->string = NULL;
    }

    if (priv->value_string) {
        g_string_free(priv->value_string, TRUE);
        priv->value_string = NULL;
    }

    G_OBJECT_CLASS(chupa_metadata_field_parent_class)->dispose(object);
}

static ChupaMetadataField *
field_new (ChupaMetadata *metadata, const gchar *name, GType type)
{
    ChupaMetadataField *field;
    ChupaMetadataFieldPrivate *priv;

    field = g_object_new(CHUPA_TYPE_METADATA_FIELD, NULL);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->metadata = g_object_ref(metadata);
    priv->name = g_strdup(name);
    priv->type = type;
    priv->string = NULL;
    priv->value_string = NULL;

    return field;
}

static ChupaMetadataField *
field_new_string (ChupaMetadata *metadata, const gchar *name, const gchar *value)
{
    ChupaMetadataField *field;
    ChupaMetadataFieldPrivate *priv;

    field = field_new(metadata, name, G_TYPE_STRING);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->value.pointer = g_strdup(value);
    priv->free_function = g_free;

    return field;
}

static ChupaMetadataField *
field_new_int (ChupaMetadata *metadata, const gchar *name, gint value)
{
    ChupaMetadataField *field;
    ChupaMetadataFieldPrivate *priv;

    field = field_new(metadata, name, G_TYPE_INT);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->value.integer = value;
    priv->free_function = NULL;

    return field;
}

static ChupaMetadataField *
field_new_time_val (ChupaMetadata *metadata, const gchar *name, GTimeVal *value)
{
    ChupaMetadataField *field;
    ChupaMetadataFieldPrivate *priv;
    GTimeVal *dupped_value;

    dupped_value = g_new0(GTimeVal, 1);
    memcpy(dupped_value, value, sizeof(GTimeVal));
    /* FIXME: use CHUPA_TYPE_TIME_VAL */
    field = field_new(metadata, name, G_TYPE_POINTER);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->value.pointer = dupped_value;
    priv->free_function = g_free;

    return field;
}

static ChupaMetadataField *
field_new_size (ChupaMetadata *metadata, const gchar *name, gsize value)
{
    ChupaMetadataField *field;
    ChupaMetadataFieldPrivate *priv;

    field = field_new(metadata, name, CHUPA_TYPE_SIZE);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->value.size = value;
    priv->free_function = NULL;

    return field;
}

const gchar *
chupa_metadata_field_name(ChupaMetadataField *field)
{
    return CHUPA_METADATA_FIELD_GET_PRIVATE(field)->name;
}

GType
chupa_metadata_field_type(ChupaMetadataField *field)
{
    return CHUPA_METADATA_FIELD_GET_PRIVATE(field)->type;
}

const gchar *
chupa_metadata_field_value_string(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->type == G_TYPE_STRING) {
        return priv->value.pointer;
    } else {
        return NULL;
    }
}

gint
chupa_metadata_field_value_int(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->type == G_TYPE_INT) {
        return priv->value.integer;
    } else {
        return 0;
    }
}

GTimeVal *
chupa_metadata_field_value_time_val(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->type == G_TYPE_POINTER) {
        return priv->value.pointer;
    } else {
        return NULL;
    }
}

gsize
chupa_metadata_field_value_size(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->type == CHUPA_TYPE_SIZE) {
        return priv->value.size;
    } else {
        return 0;
    }
}

const gchar *
chupa_metadata_field_value_as_string(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->value_string)
        return priv->value_string->str;

    priv->value_string = g_string_new(NULL);
    switch (priv->type) {
    case G_TYPE_INT:
        g_string_append_printf(priv->value_string, "%d", priv->value.integer);
        break;
    case G_TYPE_STRING:
        g_string_append(priv->value_string, priv->value.pointer);
        break;
    default:
        if (priv->type == CHUPA_TYPE_SIZE) {
            g_string_append_printf(priv->value_string, "%zd", priv->value.size);
        } else {
            g_string_append(priv->value_string, "(unsupported)");
        }
        break;
    }

    return priv->value_string->str;
}

const gchar *
chupa_metadata_field_to_string(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->string)
        return priv->string->str;

    priv->string = g_string_new(NULL);
    g_string_append(priv->string, priv->name);
    g_string_append(priv->string, ": ");
    g_string_append(priv->string, chupa_metadata_field_value_as_string(field));

    return priv->string->str;
}

#define CHUPA_METADATA_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                  \
                                 CHUPA_TYPE_METADATA,    \
                                 ChupaMetadataPrivate))

typedef struct _ChupaMetadataPrivate	ChupaMetadataPrivate;
struct _ChupaMetadataPrivate
{
    GHashTable *fields;
};

G_DEFINE_TYPE(ChupaMetadata, chupa_metadata, G_TYPE_OBJECT);

static void dispose        (GObject         *object);

static void
chupa_metadata_class_init (ChupaMetadataClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;

    g_type_class_add_private(gobject_class, sizeof(ChupaMetadataPrivate));
}

static void
chupa_metadata_init (ChupaMetadata *metadata)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);

    priv->fields = g_hash_table_new_full(g_str_hash, g_str_equal,
                                         g_free, (GDestroyNotify)g_object_unref);
}

static void
dispose (GObject *object)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(object);

    if (priv->fields) {
        g_hash_table_unref(priv->fields);
        priv->fields = NULL;
    }

    G_OBJECT_CLASS(chupa_metadata_parent_class)->dispose(object);
}

GQuark
chupa_metadata_error_quark (void)
{
    return g_quark_from_static_string("chupa-metadata-error-quark");
}

ChupaMetadata *
chupa_metadata_new(void)
{
    return g_object_new(CHUPA_TYPE_METADATA,
                        NULL);
}

void
chupa_metadata_merge_original_metadata (ChupaMetadata *metadata,
                                        ChupaMetadata *original)
{
    const gchar *original_filename, *original_mime_type;
    gsize original_content_length;

    original_filename = chupa_metadata_get_original_filename(original);
    if (!original_filename)
        original_filename = chupa_metadata_get_filename(original);
    if (original_filename)
        chupa_metadata_set_original_filename(metadata, original_filename);

    original_mime_type = chupa_metadata_get_original_mime_type(original);
    if (!original_mime_type)
        original_mime_type = chupa_metadata_get_mime_type(original);
    if (original_mime_type)
        chupa_metadata_set_original_mime_type(metadata, original_mime_type);

    original_content_length =
        chupa_metadata_get_original_content_length(original);
    if (original_content_length == 0)
        original_content_length = chupa_metadata_get_content_length(original);
    if (original_content_length > 0)
        chupa_metadata_set_original_content_length(metadata,
                                                   original_content_length);
}

guint
chupa_metadata_size(ChupaMetadata *metadata)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    return g_hash_table_size(priv->fields);
}

typedef struct _FieldForeachData
{
    ChupaMetadataFieldCallback callback;
    gpointer user_data;
} FieldForeachData;

static void
fields_foreach_body(gpointer key, gpointer value, gpointer user_data)
{
    ChupaMetadataField *field = value;
    FieldForeachData *data = user_data;

    data->callback(field, data->user_data);
}

void
chupa_metadata_foreach(ChupaMetadata *metadata,
                       ChupaMetadataFieldCallback callback, gpointer user_data)
{
    ChupaMetadataPrivate *priv;
    FieldForeachData data;

    data.callback = callback;
    data.user_data = user_data;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    g_hash_table_foreach(priv->fields, fields_foreach_body, &data);
}

static ChupaMetadataField *
field_lookup (ChupaMetadata *metadata, const gchar *key, GError **error)
{
    ChupaMetadataPrivate *priv;
    ChupaMetadataField *field;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    field = g_hash_table_lookup(priv->fields, key);
    if (!field) {
        g_set_error(error,
                    CHUPA_METADATA_ERROR,
                    CHUPA_METADATA_ERROR_NOT_EXIST,
                    "requested key doesn't exist in metadata: <%s>",
                    key);
        return NULL;
    }

    return field;
}

gboolean
chupa_metadata_remove (ChupaMetadata *metadata, const gchar *key)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    return g_hash_table_remove(priv->fields, key);
}

void
chupa_metadata_set_string (ChupaMetadata *metadata, const gchar *key,
                           const gchar *value)
{
    ChupaMetadataField *field;
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);

    field = field_new_string(metadata, key, value);
    g_hash_table_insert(priv->fields, g_strdup(key), field);
}

const gchar *
chupa_metadata_get_string (ChupaMetadata *metadata, const gchar *key,
                           GError **error)
{
    ChupaMetadataField *field;

    field = field_lookup(metadata, key, error);
    if (!field) {
        return 0;
    }

    return chupa_metadata_field_value_string(field);
}

void
chupa_metadata_set_int (ChupaMetadata *metadata, const gchar *key, gint value)
{
    ChupaMetadataField *field;
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);

    field = field_new_int(metadata, key, value);
    g_hash_table_insert(priv->fields, g_strdup(key), field);
}

gint
chupa_metadata_get_int (ChupaMetadata *metadata, const gchar *key, GError **error)
{
    ChupaMetadataField *field;

    field = field_lookup(metadata, key, error);
    if (!field) {
        return 0;
    }

    return chupa_metadata_field_value_int(field);
}

void
chupa_metadata_set_time_val (ChupaMetadata *metadata, const gchar *key,
                             GTimeVal *value)
{
    ChupaMetadataField *field;
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    field = field_new_time_val(metadata, key, value);
    g_hash_table_insert(priv->fields, g_strdup(key), field);
}

GTimeVal *
chupa_metadata_get_time_val (ChupaMetadata *metadata, const gchar *key, GError **error)
{
    ChupaMetadataField *field;

    field = field_lookup(metadata, key, error);
    if (!field) {
        return NULL;
    }

    return chupa_metadata_field_value_time_val(field);
}

void
chupa_metadata_set_size (ChupaMetadata *metadata, const gchar *key, gsize size)
{
    ChupaMetadataField *field;
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    field = field_new_size(metadata, key, size);
    g_hash_table_insert(priv->fields, g_strdup(key), field);
}

gsize
chupa_metadata_get_size (ChupaMetadata *metadata, const gchar *key, GError **error)
{
    ChupaMetadataField *field;

    field = field_lookup(metadata, key, error);
    if (!field) {
        return 0;
    }

    return chupa_metadata_field_value_size(field);
}

void
chupa_metadata_set_content_length (ChupaMetadata *metadata, gsize length)
{
    chupa_metadata_set_size(metadata, CHUPA_METADATA_NAME_CONTENT_LENGTH,
                            length);
}

gsize
chupa_metadata_get_content_length (ChupaMetadata *metadata)
{
    return chupa_metadata_get_size(metadata, CHUPA_METADATA_NAME_CONTENT_LENGTH,
                                   NULL);
}

void
chupa_metadata_set_original_content_length (ChupaMetadata *metadata,
                                            gsize length)
{
    chupa_metadata_set_size(metadata,
                            CHUPA_METADATA_NAME_ORIGINAL_CONTENT_LENGTH,
                            length);
}

gsize
chupa_metadata_get_original_content_length (ChupaMetadata *metadata)
{
    return chupa_metadata_get_size(metadata,
                                   CHUPA_METADATA_NAME_ORIGINAL_CONTENT_LENGTH,
                                   NULL);
}

void
chupa_metadata_set_filename (ChupaMetadata *metadata, const gchar *filename)
{
    chupa_metadata_set_string(metadata, CHUPA_METADATA_NAME_FILENAME, filename);
}

const gchar *
chupa_metadata_get_filename (ChupaMetadata *metadata)
{
    return chupa_metadata_get_string(metadata, CHUPA_METADATA_NAME_FILENAME,
                                     NULL);
}

void
chupa_metadata_set_original_filename (ChupaMetadata *metadata,
                                      const gchar *filename)
{
    chupa_metadata_set_string(metadata, CHUPA_METADATA_NAME_ORIGINAL_FILENAME,
                              filename);
}

const gchar *
chupa_metadata_get_original_filename (ChupaMetadata *metadata)
{
    return chupa_metadata_get_string(metadata,
                                     CHUPA_METADATA_NAME_ORIGINAL_FILENAME,
                                     NULL);
}

void
chupa_metadata_set_mime_type (ChupaMetadata *metadata, const gchar *mime_type)
{
    chupa_metadata_set_string(metadata, CHUPA_METADATA_NAME_MIME_TYPE,
                              mime_type);
}

const gchar *
chupa_metadata_get_mime_type (ChupaMetadata *metadata)
{
    return chupa_metadata_get_string(metadata, CHUPA_METADATA_NAME_MIME_TYPE,
                                     NULL);
}

void
chupa_metadata_set_original_mime_type (ChupaMetadata *metadata,
                                       const gchar *mime_type)
{
    chupa_metadata_set_string(metadata, CHUPA_METADATA_NAME_ORIGINAL_MIME_TYPE,
                              mime_type);
}

const gchar *
chupa_metadata_get_original_mime_type (ChupaMetadata *metadata)
{
    return chupa_metadata_get_string(metadata,
                                     CHUPA_METADATA_NAME_ORIGINAL_MIME_TYPE,
                                     NULL);
}

const gchar *
chupa_metadata_get_encoding (ChupaMetadata *metadata)
{
    return chupa_metadata_get_string(metadata, CHUPA_METADATA_NAME_ENCODING,
                                     NULL);
}

void
chupa_metadata_set_original_encoding (ChupaMetadata *metadata,
                                      const gchar *encoding)
{
    chupa_metadata_set_string(metadata, CHUPA_METADATA_NAME_ORIGINAL_ENCODING,
                              encoding);
}

const gchar *
chupa_metadata_get_original_encoding (ChupaMetadata *metadata)
{
    return chupa_metadata_get_string(metadata,
                                     CHUPA_METADATA_NAME_ORIGINAL_ENCODING,
                                     NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
