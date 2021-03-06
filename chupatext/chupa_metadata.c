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
#include "chupa_types.h"

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
        gboolean boolean;
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

    field = field_new(metadata, name, CHUPA_TYPE_TIME_VAL);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->value.pointer = chupa_time_val_dup(value);
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

static ChupaMetadataField *
field_new_boolean (ChupaMetadata *metadata, const gchar *name, gboolean value)
{
    ChupaMetadataField *field;
    ChupaMetadataFieldPrivate *priv;

    field = field_new(metadata, name, G_TYPE_BOOLEAN);
    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    priv->value.boolean = value;
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
    if (priv->type == CHUPA_TYPE_TIME_VAL) {
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

gboolean
chupa_metadata_field_value_boolean(ChupaMetadataField *field)
{
    ChupaMetadataFieldPrivate *priv;

    priv = CHUPA_METADATA_FIELD_GET_PRIVATE(field);
    if (priv->type == G_TYPE_BOOLEAN) {
        return priv->value.boolean;
    } else {
        return FALSE;
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
    case G_TYPE_BOOLEAN:
        g_string_append(priv->value_string,
                        priv->value.boolean ? "true" : "false");
        break;
    default:
        if (priv->type == CHUPA_TYPE_SIZE) {
            g_string_append_printf(priv->value_string, "%zd", priv->value.size);
        } else if (priv->type == CHUPA_TYPE_TIME_VAL) {
            gchar *iso_8601;
            iso_8601 = g_time_val_to_iso8601(priv->value.pointer);
            g_string_append(priv->value_string, iso_8601);
            g_free(iso_8601);
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

enum {
    PROP_0,
    PROP_MIME_TYPE,
    PROP_ENCODING,
    PROP_CONTENT_LENGTH,
    PROP_FILENAME,
    PROP_PATH,
    PROP_ORIGINAL_MIME_TYPE,
    PROP_ORIGINAL_ENCODING,
    PROP_ORIGINAL_CONTENT_LENGTH,
    PROP_ORIGINAL_FILENAME,
    PROP_TITLE,
    PROP_AUTHOR,
    PROP_MODIFICATION_TIME,
    PROP_CREATION_TIME,
    PROP_META_IGNORE_TIME,
    PROP_LAST
};

G_DEFINE_TYPE(ChupaMetadata, chupa_metadata, G_TYPE_OBJECT);

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
#  define g_param_spec_size g_param_spec_ulong
#  define g_value_set_size g_value_set_ulong
#  define g_value_get_size g_value_get_ulong
#else
#  define g_param_spec_size g_param_spec_uint64
#  define g_value_set_size g_value_set_uint64
#  define g_value_get_size g_value_get_uint64
#endif

static void
chupa_metadata_class_init (ChupaMetadataClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("mime-type",
                               "MIME type",
                               "MIME type of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_MIME_TYPE, spec);

    spec = g_param_spec_string("encoding",
                               "Encoding",
                               "Encoding of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_ENCODING, spec);

    spec = g_param_spec_size("content-length",
                             "Content length",
                             "Content length of the associated data",
                             0,
                             G_MAXSIZE,
                             0,
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_CONTENT_LENGTH, spec);

    spec = g_param_spec_string("filename",
                               "Filename",
                               "Filename of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_FILENAME, spec);

    spec = g_param_spec_string("path",
                               "Path",
                               "Path of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_PATH, spec);

    spec = g_param_spec_string("original-mime-type",
                               "Original MIME type",
                               "Original MIME type of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_ORIGINAL_MIME_TYPE,
                                    spec);

    spec = g_param_spec_string("original-encoding",
                               "Original encoding",
                               "Original encoding of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_ORIGINAL_ENCODING, spec);

    spec = g_param_spec_size("orignal-content-length",
                             "Original content length",
                             "Original content length of the associated data",
                             0,
                             G_MAXSIZE,
                             0,
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_ORIGINAL_CONTENT_LENGTH,
                                    spec);

    spec = g_param_spec_string("original-filename",
                               "Original filename",
                               "Original filename of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_ORIGINAL_FILENAME, spec);

    spec = g_param_spec_string("title",
                               "Title",
                               "Tile of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_TITLE, spec);

    spec = g_param_spec_string("author",
                               "Author",
                               "Author of the associated data",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_AUTHOR, spec);

    spec = chupa_param_spec_time_val("modification-time",
                                     "Modification time",
                                     "Modification time of the associated data",
                                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_MODIFICATION_TIME, spec);

    spec = chupa_param_spec_time_val("creation-time",
                                     "Creation time",
                                     "Creation time of the associated data",
                                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_CREATION_TIME, spec);

    spec = g_param_spec_boolean("meta-ignore-time",
                                "Meta: ignore time",
                                "Meta: whether ignore time values "
                                "of the associated data",
                                FALSE,
                                G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_META_IGNORE_TIME, spec);


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

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaMetadata *metadata;

    metadata = CHUPA_METADATA(object);
    switch (prop_id) {
    case PROP_MIME_TYPE:
        chupa_metadata_set_mime_type(metadata, g_value_get_string(value));
        break;
    case PROP_ENCODING:
        chupa_metadata_set_encoding(metadata, g_value_get_string(value));
        break;
    case PROP_CONTENT_LENGTH:
        chupa_metadata_set_content_length(metadata, g_value_get_size(value));
        break;
    case PROP_FILENAME:
        chupa_metadata_set_filename(metadata, g_value_get_string(value));
        break;
    case PROP_PATH:
        chupa_metadata_set_path(metadata, g_value_get_string(value));
        break;
    case PROP_ORIGINAL_MIME_TYPE:
        chupa_metadata_set_original_mime_type(metadata,
                                              g_value_get_string(value));
        break;
    case PROP_ORIGINAL_ENCODING:
        chupa_metadata_set_original_encoding(metadata,
                                             g_value_get_string(value));
        break;
    case PROP_ORIGINAL_CONTENT_LENGTH:
        chupa_metadata_set_original_content_length(metadata,
                                                   g_value_get_size(value));
        break;
    case PROP_ORIGINAL_FILENAME:
        chupa_metadata_set_original_filename(metadata,
                                             g_value_get_string(value));
        break;
    case PROP_TITLE:
        chupa_metadata_set_title(metadata, g_value_get_string(value));
        break;
    case PROP_AUTHOR:
        chupa_metadata_set_author(metadata, g_value_get_string(value));
        break;
    case PROP_MODIFICATION_TIME:
        chupa_metadata_set_modification_time(metadata,
                                             chupa_value_get_time_val(value));
        break;
    case PROP_CREATION_TIME:
        chupa_metadata_set_creation_time(metadata,
                                         chupa_value_get_time_val(value));
        break;
    case PROP_META_IGNORE_TIME:
        chupa_metadata_set_meta_ignore_time(metadata,
                                            g_value_get_boolean(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property(GObject *object,
             guint prop_id,
             GValue *value,
             GParamSpec *pspec)
{
    ChupaMetadata *metadata;

    metadata = CHUPA_METADATA(object);
    switch (prop_id) {
    case PROP_MIME_TYPE:
        g_value_set_string(value, chupa_metadata_get_mime_type(metadata));
        break;
    case PROP_ENCODING:
        g_value_set_string(value, chupa_metadata_get_encoding(metadata));
        break;
    case PROP_CONTENT_LENGTH:
        g_value_set_size(value, chupa_metadata_get_content_length(metadata));
        break;
    case PROP_FILENAME:
        g_value_set_string(value, chupa_metadata_get_filename(metadata));
        break;
    case PROP_PATH:
        g_value_set_string(value, chupa_metadata_get_path(metadata));
        break;
    case PROP_ORIGINAL_MIME_TYPE:
        g_value_set_string(value,
                           chupa_metadata_get_original_mime_type(metadata));
        break;
    case PROP_ORIGINAL_ENCODING:
        g_value_set_string(value,
                           chupa_metadata_get_original_encoding(metadata));
        break;
    case PROP_ORIGINAL_CONTENT_LENGTH:
        g_value_set_size(value,
                         chupa_metadata_get_original_content_length(metadata));
        break;
    case PROP_ORIGINAL_FILENAME:
        g_value_set_string(value,
                           chupa_metadata_get_original_filename(metadata));
        break;
    case PROP_TITLE:
        g_value_set_string(value, chupa_metadata_get_title(metadata));
        break;
    case PROP_AUTHOR:
        g_value_set_string(value, chupa_metadata_get_author(metadata));
        break;
    case PROP_MODIFICATION_TIME:
        chupa_value_set_time_val(value,
                                 chupa_metadata_get_modification_time(metadata));
        break;
    case PROP_CREATION_TIME:
        chupa_value_set_time_val(value,
                                 chupa_metadata_get_creation_time(metadata));
        break;
    case PROP_META_IGNORE_TIME:
        g_value_set_boolean(value,
                            chupa_metadata_get_meta_ignore_time(metadata));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
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

    if (!chupa_metadata_get_path(metadata))
        chupa_metadata_set_path(metadata, chupa_metadata_get_path(original));

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
chupa_metadata_set_size (ChupaMetadata *metadata, const gchar *key, gsize value)
{
    ChupaMetadataField *field;
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    field = field_new_size(metadata, key, value);
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
chupa_metadata_set_boolean (ChupaMetadata *metadata,
                            const gchar *key, gboolean value)
{
    ChupaMetadataField *field;
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    field = field_new_boolean(metadata, key, value);
    g_hash_table_insert(priv->fields, g_strdup(key), field);
}

gboolean
chupa_metadata_get_boolean (ChupaMetadata *metadata,
                            const gchar *key, GError **error)
{
    ChupaMetadataField *field;

    field = field_lookup(metadata, key, error);
    if (!field) {
        return FALSE;
    }

    return chupa_metadata_field_value_boolean(field);
}

#define DEFINE_ACCESSOR(name, key_name, type, type_name)                \
void                                                                    \
chupa_metadata_set_ ## name (ChupaMetadata *metadata, type name)        \
{                                                                       \
    const gchar *key = CHUPA_METADATA_NAME_ ## key_name;                \
    chupa_metadata_set_ ## type_name(metadata, key, name);              \
}                                                                       \
                                                                        \
type                                                                    \
chupa_metadata_get_ ## name (ChupaMetadata *metadata)                   \
{                                                                       \
    const gchar *key = CHUPA_METADATA_NAME_ ## key_name;                \
    return chupa_metadata_get_ ## type_name(metadata, key, NULL);       \
}

#define DEFINE_SIZE_ACCESSOR(name, key_name)            \
    DEFINE_ACCESSOR(name, key_name, gsize, size)
#define DEFINE_STRING_ACCESSOR(name, key_name)                  \
    DEFINE_ACCESSOR(name, key_name, const gchar *, string)
#define DEFINE_BOOLEAN_ACCESSOR(name, key_name)         \
    DEFINE_ACCESSOR(name, key_name, gboolean, boolean)
#define DEFINE_TIME_VAL_ACCESSOR(name, key_name)         \
    DEFINE_ACCESSOR(name, key_name, GTimeVal *, time_val)

DEFINE_SIZE_ACCESSOR(content_length, CONTENT_LENGTH)
DEFINE_SIZE_ACCESSOR(original_content_length, ORIGINAL_CONTENT_LENGTH)
DEFINE_STRING_ACCESSOR(path, PATH)
DEFINE_STRING_ACCESSOR(filename, FILENAME)
DEFINE_STRING_ACCESSOR(original_filename, ORIGINAL_FILENAME)
DEFINE_STRING_ACCESSOR(mime_type, MIME_TYPE)
DEFINE_STRING_ACCESSOR(original_mime_type, ORIGINAL_MIME_TYPE)
DEFINE_STRING_ACCESSOR(encoding, ENCODING)
DEFINE_STRING_ACCESSOR(original_encoding, ORIGINAL_ENCODING)
DEFINE_STRING_ACCESSOR(title, TITLE)
DEFINE_STRING_ACCESSOR(author, AUTHOR)
DEFINE_TIME_VAL_ACCESSOR(modification_time, MODIFICATION_TIME)
DEFINE_TIME_VAL_ACCESSOR(creation_time, CREATION_TIME)
DEFINE_BOOLEAN_ACCESSOR(meta_ignore_time, META_IGNORE_TIME)

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
