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

#include <string.h>

#include <chupatext/chupa_decomposer_module.h>

#include <glib.h>
#include <glib/poppler.h>

/* ChupaPDFDecomposer */
#define CHUPA_TYPE_PDF_DECOMPOSER \
    (chupa_type_pdf_decomposer)
#define CHUPA_PDF_DECOMPOSER(obj)                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                CHUPA_TYPE_PDF_DECOMPOSER,      \
                                ChupaPDFDecomposer))
#define CHUPA_PDF_DECOMPOSER_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                   \
                             CHUPA_TYPE_PDF_DECOMPOSER, \
                             ChupaPDFDecomposerClass)
#define CHUPA_IS_PDF_DECOMPOSER(obj)                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                CHUPA_TYPE_PDF_DECOMPOSER))
#define CHUPA_IS_PDF_DECOMPOSER_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             CHUPA_TYPE_PDF_DECOMPOSER))
#define CHUPA_PDF_DECOMPOSER_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               CHUPA_TYPE_PDF_DECOMPOSER,       \
                               ChupaPDFDecomposerClass)

typedef struct _ChupaPDFDecomposer ChupaPDFDecomposer;
typedef struct _ChupaPDFDecomposerClass ChupaPDFDecomposerClass;

struct _ChupaPDFDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaPDFDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_pdf_decomposer = 0;

static void
show_all_layers_recursive(PopplerLayersIter *iter)
{
    do {
        PopplerLayer *layer;
        PopplerLayersIter *child;

        layer = poppler_layers_iter_get_layer(iter);
        if (layer) {
            if (!poppler_layer_is_visible(layer))
                poppler_layer_show(layer);
            g_object_unref(layer);
        }
        child = poppler_layers_iter_get_child(iter);
        if (child) {
            show_all_layers_recursive(child);
            poppler_layers_iter_free(child);
        }
    } while (poppler_layers_iter_next(iter));
}

static void
show_all_layers(PopplerDocument *document)
{
    PopplerLayersIter *iter;

    iter = poppler_layers_iter_new(document);
    if (iter) {
        show_all_layers_recursive(iter);
        poppler_layers_iter_free(iter);
    }
}

static gboolean
feed(ChupaDecomposer *dec, ChupaFeeder *feeder, ChupaData *data, GError **error)
{
    PopplerDocument *doc;
    GMemoryInputStream *memory_input = NULL;
    gssize count;
    gsize content_length = 0;
    gchar buffer[16*1024];
    const gsize bufsize = sizeof(buffer);
    GString *str = g_string_sized_new(sizeof(buffer));
    GInputStream *input = chupa_data_get_stream(data);
    ChupaMetadata *input_metadata, *output_metadata;
    ChupaData *pdf_text = NULL;
    gchar *title, *author, *metadata;
    gint creation_time, modification_time;
    int n, i;

    input_metadata = chupa_data_get_metadata(data);
    while ((count = g_input_stream_read(input, buffer, bufsize, NULL, error)) > 0) {
        g_string_append_len(str, buffer, count);
        if (count < bufsize) break;
    }
    if (count < 0 || !str->len) {
        g_string_free(str, TRUE);
        return FALSE;
    }
    doc = poppler_document_new_from_data(str->str, str->len, NULL, error);
    if (!doc) {
        g_string_free(str, TRUE);
        return FALSE;
    }

    output_metadata = chupa_metadata_new();
    chupa_metadata_merge_original_metadata(output_metadata, input_metadata);
    g_object_get(doc,
                 "title", &title,
                 "author", &author,
                 "creation-date", &creation_time,
                 "mod-date", &modification_time,
                 "metadata", &metadata,
                 NULL);
    if (title) {
        chupa_metadata_set_title(output_metadata, title);
        g_free(title);
    }
    if (author) {
        chupa_metadata_set_author(output_metadata, author);
        g_free(author);
    }
    if (!chupa_metadata_get_meta_ignore_time(input_metadata)) {
        GTimeVal time;
        time.tv_usec = 0;
        if (creation_time > 0) {
            time.tv_sec = creation_time;
            chupa_metadata_set_creation_time(output_metadata, &time);
        }
        if (modification_time > 0) {
            time.tv_sec = modification_time;
            chupa_metadata_set_modification_time(output_metadata, &time);
        }
    }
    if (metadata) {
        chupa_metadata_set_string(output_metadata, "metadata", metadata);
        g_free(metadata);
    }
    show_all_layers(doc);
    n = poppler_document_get_n_pages(doc);
    for (i = 0; i < n; ++i) {
        PopplerPage *page = poppler_document_get_page(doc, i);
        gchar *text;
#if POPPLER_CHECK_VERSION(0, 15, 0)
        text = poppler_page_get_text(page);
#else
        PopplerRectangle rectangle = {0, 0, 0, 0};
        poppler_page_get_size(page, &rectangle.x2, &rectangle.y2);
        text = poppler_page_get_text(page, POPPLER_SELECTION_GLYPH, &rectangle);
#endif
        content_length += strlen(text);
        if (memory_input) {
            g_memory_input_stream_add_data(memory_input, "\f", 1, NULL);
            content_length++;
            g_memory_input_stream_add_data(memory_input, text, -1, g_free);
            chupa_metadata_set_content_length(output_metadata, content_length);
        } else {
            const gchar *filename;
            filename = chupa_data_get_filename(data);
            input = g_memory_input_stream_new_from_data(text, -1, g_free);
            chupa_metadata_set_content_length(output_metadata, content_length);
            pdf_text = chupa_data_new(input, output_metadata);
            chupa_feeder_accepted(feeder, pdf_text);
            memory_input = (GMemoryInputStream *)input;
        }
        g_object_unref(page);
    }
    g_object_unref(output_metadata);
    g_object_unref(doc);
    g_string_free(str, TRUE);
    chupa_data_finished(pdf_text, NULL);
    g_object_unref(memory_input);
    g_object_unref(pdf_text);
    return TRUE;
}

static void
decomposer_class_init(ChupaPDFDecomposerClass *klass)
{
    ChupaDecomposerClass *decomposer_class;

    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);
    decomposer_class->feed = feed;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaPDFDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaPDFDecomposer),
        0,
        (GInstanceInitFunc)NULL,
    };
    const gchar *type_name = "ChupaPDFDecomposer";

    chupa_type_pdf_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}


/* ChupaPDFDecomposerFactory */
#define CHUPA_TYPE_PDF_DECOMPOSER_FACTORY \
    (chupa_type_pdf_decomposer_factory)
#define CHUPA_PDF_DECOMPOSER_FACTORY(obj)                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_PDF_DECOMPOSER_FACTORY,      \
                                ChupaPDFDecomposerFactory))
#define CHUPA_PDF_DECOMPOSER_FACTORY_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             CHUPA_TYPE_PDF_DECOMPOSER_FACTORY, \
                             ChupaPDFDecomposerFactoryClass))
#define CHUPA_IS_PDF_DECOMPOSER_FACTORY(obj)                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_PDF_DECOMPOSER_FACTORY))
#define CHUPA_IS_PDF_DECOMPOSER_FACTORY_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_PDF_DECOMPOSER_FACTORY))
#define CHUPA_PDF_DECOMPOSER_FACTORY_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_PDF_DECOMPOSER_FACTORY,       \
                               ChupaPDFDecomposerFactoryClass))

typedef struct _ChupaPDFDecomposerFactory ChupaPDFDecomposerFactory;
typedef struct _ChupaPDFDecomposerFactoryClass ChupaPDFDecomposerFactoryClass;

struct _ChupaPDFDecomposerFactory
{
    ChupaDecomposerFactory     object;
};

struct _ChupaPDFDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_pdf_decomposer_factory = 0;
static ChupaDecomposerFactoryClass *factory_parent_class;

static GList     *get_mime_types   (ChupaDecomposerFactory *factory);
static GObject   *create           (ChupaDecomposerFactory *factory,
                                    const gchar            *label,
                                    const gchar            *mime_type,
                                    GError                **error);

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
            sizeof (ChupaPDFDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaPDFDecomposerFactory),
            0,
            (GInstanceInitFunc)NULL,
        };
    const gchar *type_name = "ChupaPDFDecomposerFactory";

    chupa_type_pdf_decomposer_factory =
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

    mime_types = g_list_prepend(mime_types, g_strdup("application/pdf"));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label,
       const gchar *mime_type, GError **error)
{
    return g_object_new(CHUPA_TYPE_PDF_DECOMPOSER,
                        "mime-type", mime_type,
                        NULL);
}

/* module entry points */
G_MODULE_EXPORT GList *
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module, GError **error)
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
    return g_object_new_valist(CHUPA_TYPE_PDF_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
