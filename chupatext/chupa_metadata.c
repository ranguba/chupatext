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

#include "chupatext/chupa_metadata.h"

#define CHUPA_METADATA_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                 \
                                 CHUPA_TYPE_METADATA,    \
                                 ChupaMetadataPrivate))

typedef struct _ChupaMetadataPrivate	ChupaMetadataPrivate;
struct _ChupaMetadataPrivate
{
    GHashTable *data;
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
data_values_free(gpointer pointer)
{
    GList *values = pointer;
    g_list_foreach(values, (GFunc)g_free, NULL);
    g_list_free(values);
}

static void
chupa_metadata_init (ChupaMetadata *metadata)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);

    priv->data = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, data_values_free);
}

static void
dispose (GObject *object)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(object);

    if (priv->data) {
        g_hash_table_unref(priv->data);
        priv->data = NULL;
    }

    G_OBJECT_CLASS(chupa_metadata_parent_class)->dispose(object);
}

ChupaMetadata *
chupa_metadata_new(void)
{
    return g_object_new(CHUPA_TYPE_METADATA,
                        NULL);
}

void
chupa_metadata_add_value(ChupaMetadata *metadata, const gchar *key, const gchar *value)
{
    ChupaMetadataPrivate *priv;
    GList *values = NULL;
    gpointer keyptr = (gpointer)key, valptr;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    if (g_hash_table_lookup_extended(priv->data, key, &keyptr, &valptr)) {
        g_hash_table_steal(priv->data, keyptr);
        key = keyptr;
        values = valptr;
    }
    else {
        key = g_strdup(key);
    }
    values = g_list_append(values, g_strdup(value));
    g_hash_table_insert(priv->data, (gpointer)key, values);
}

void
chupa_metadata_replace_value(ChupaMetadata *metadata, const gchar *key, const gchar *value)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    if (!value) {
        g_hash_table_remove(priv->data, key);
        return;
    }
    g_hash_table_replace(priv->data, g_strdup(key), g_list_append(NULL, g_strdup(value)));
}

const gchar *
chupa_metadata_get_first_value(ChupaMetadata *metadata, const gchar *key)
{
    ChupaMetadataPrivate *priv;
    GList *values;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    values = g_hash_table_lookup(priv->data, key);
    if (!values) {
        return NULL;
    }
    return values->data;
}

GList *
chupa_metadata_get_values(ChupaMetadata *metadata, const gchar *key)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    return g_hash_table_lookup(priv->data, key);
}

static void
list_copy(gpointer value, gpointer user_data)
{
    GList **dest = user_data;
    *dest = g_list_append(*dest, g_strdup(value));
}

static void
metadata_update_func(gpointer key, gpointer value, gpointer user_data)
{
    GHashTable *dest = user_data;
    GList *l1 = g_hash_table_lookup(dest, key), *l2 = value;

    if (g_hash_table_lookup_extended(dest, key, &key, &value)) {
        g_hash_table_steal(dest, key);
    }
    else {
        key = g_strdup(key);
    }
    g_list_foreach(l2, list_copy, &l1);
    g_hash_table_insert(dest, key, l1);
}

void
chupa_metadata_update(ChupaMetadata *metadata, ChupaMetadata *update)
{
    ChupaMetadataPrivate *priv1, *priv2;

    priv1 = CHUPA_METADATA_GET_PRIVATE(metadata);
    priv2 = CHUPA_METADATA_GET_PRIVATE(update);
    g_hash_table_foreach(priv2->data, metadata_update_func, priv1->data);
}

guint
chupa_metadata_size(ChupaMetadata *metadata)
{
    ChupaMetadataPrivate *priv;

    priv = CHUPA_METADATA_GET_PRIVATE(metadata);
    return g_hash_table_size(priv->data);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
