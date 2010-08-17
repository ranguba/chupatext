/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include "chupatext/config.h"
#endif /* HAVE_CONFIG_H */

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
    GParamSpec *spec;

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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
