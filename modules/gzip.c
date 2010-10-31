/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

#include <string.h>
#include <chupatext/chupa_decomposer_module.h>
#include <glib.h>
#include <gsf/gsf-input-gzip.h>

#include "chupatext/chupa_gsf_input_stream.h"
#include "chupatext/chupa_data_input.h"

/* ChupaGzipDecomposer */
#define CHUPA_TYPE_GZIP_DECOMPOSER               \
    chupa_type_gzip_decomposer
#define CHUPA_GZIP_DECOMPOSER(obj)                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                CHUPA_TYPE_GZIP_DECOMPOSER,      \
                                ChupaGzipDecomposer))
#define CHUPA_GZIP_DECOMPOSER_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                   \
                             CHUPA_TYPE_GZIP_DECOMPOSER, \
                             ChupaGzipDecomposerClass))
#define CHUPA_IS_GZIP_DECOMPOSER(obj)                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                CHUPA_TYPE_GZIP_DECOMPOSER))
#define CHUPA_IS_GZIP_DECOMPOSER_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             CHUPA_TYPE_GZIP_DECOMPOSER))
#define CHUPA_GZIP_DECOMPOSER_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               CHUPA_TYPE_GZIP_DECOMPOSER,       \
                               ChupaGzipDecomposerClass)

typedef struct _ChupaGzipDecomposer ChupaGzipDecomposer;
typedef struct _ChupaGzipDecomposerClass ChupaGzipDecomposerClass;

struct _ChupaGzipDecomposer
{
    ChupaDecomposer parent_object;
};

struct _ChupaGzipDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_gzip_decomposer = 0;

static gboolean
feed(ChupaDecomposer *dec, ChupaFeeder *feeder, ChupaData *data, GError **err)
{
    GsfInput *gzip_input, *input;
    ChupaGsfInputStream *stream;
    ChupaMetadata *metadata;
    const gchar *filename;
    GString *new_filename;
    gboolean result;

    g_return_val_if_fail(CHUPA_IS_DECOMPOSER(dec), FALSE);
    g_return_val_if_fail(CHUPA_IS_DATA(data), FALSE);
    filename = chupa_data_get_filename(data);
    input = chupa_data_input_new(data);
    gzip_input = gsf_input_gzip_new(input, err);
    if (!gzip_input) {
        return FALSE;
    }
    stream = chupa_gsf_input_stream_new(gzip_input);
    g_object_unref(gzip_input);

    if (g_str_has_suffix(filename, ".gz")){
        new_filename = g_string_new_len(filename, strlen(filename) - 3);
    } else if (g_str_has_suffix(filename, ".tgz")){
        new_filename = g_string_new_len(filename, strlen(filename) - 3);
        g_string_append(new_filename, "tar");
    } else {
        return FALSE;
    }
    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "filename", new_filename->str);
    g_string_free(new_filename, TRUE);
    chupa_metadata_set_content_length(metadata, gsf_input_size(gzip_input));
    data = chupa_data_new(G_INPUT_STREAM(stream), metadata);
    g_object_unref(metadata);
    g_object_unref(stream);
    result = chupa_feeder_feed(feeder, data, err);
    g_object_unref(data);
    return result;
}

static void
decomposer_class_init(ChupaGzipDecomposerClass *klass)
{
    ChupaDecomposerClass *decomposer_class;

    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);
    decomposer_class->feed = feed;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaGzipDecomposerClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaGzipDecomposer),
        0,
        (GInstanceInitFunc)NULL,
    };
    const gchar *type_name = "ChupaGzipDecomposer";

    chupa_type_gzip_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

/* ChupaGzipDecomposerFactory */
#define CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY       \
    (chupa_type_gzip_decomposer_factory)
#define CHUPA_GZIP_DECOMPOSER_FACTORY(obj)                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY,      \
                                ChupaGzipDecomposerFactory))
#define CHUPA_GZIP_DECOMPOSER_FACTORY_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY, \
                             ChupaGzipDecomposerFactoryClass))
#define CHUPA_IS_GZIP_DECOMPOSER_FACTORY(obj)                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY))
#define CHUPA_IS_GZIP_DECOMPOSER_FACTORY_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY))
#define CHUPA_GZIP_DECOMPOSER_FACTORY_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY,       \
                               ChupaGzipDecomposerFactoryClass))

typedef struct _ChupaGzipDecomposerFactory ChupaGzipDecomposerFactory;
typedef struct _ChupaGzipDecomposerFactoryClass ChupaGzipDecomposerFactoryClass;

struct _ChupaGzipDecomposerFactory
{
    ChupaDecomposerFactory     object;
};

struct _ChupaGzipDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_gzip_decomposer_factory = 0;
static ChupaDecomposerFactoryClass *factory_parent_class;

static GList     *get_mime_types   (ChupaDecomposerFactory *factory);
static GObject   *create           (ChupaDecomposerFactory *factory,
                                    const gchar            *label,
                                    const gchar            *mime_type);

static void
factory_class_init(ChupaDecomposerFactoryClass *klass)
{
    GObjectClass *gobject_class;
    ChupaDecomposerFactoryClass *factory_class;

    factory_parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class = CHUPA_DECOMPOSER_FACTORY_CLASS(klass);

    factory_class->get_mime_types   = get_mime_types;
    factory_class->create           = create;
}

static void
factory_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info =
        {
            sizeof (ChupaGzipDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaGzipDecomposerFactory),
            0,
            (GInstanceInitFunc)NULL,
        };
    const gchar *type_name = "ChupaGzipDecomposerFactory";

    chupa_type_gzip_decomposer_factory =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER_FACTORY,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

static GList *
get_mime_types(ChupaDecomposerFactory *factory)
{
    GList *mime_types = NULL;

    mime_types = g_list_prepend(mime_types, g_strdup("application/compressed-tar"));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label, const gchar *mime_type)
{
    return g_object_new(CHUPA_TYPE_GZIP_DECOMPOSER,
                        "mime-type", mime_type,
                        NULL);
}

/* module entry points */
G_MODULE_EXPORT GList *
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    return registered_types;
}

G_MODULE_EXPORT gboolean
CHUPA_DECOMPOSER_QUIT(void)
{
    return TRUE;
}

G_MODULE_EXPORT GObject *
CHUPA_DECOMPOSER_CREATE_FACTORY(const gchar *first_property, va_list va_args)
{
    return g_object_new_valist(CHUPA_TYPE_GZIP_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
