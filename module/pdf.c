/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module.h>
#include <glib.h>
#include <glib/poppler.h>

#define CHUPA_TYPE_PDF_DECOMPOSER            chupa_type_pdf_decomposer
#define CHUPA_PDF_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_PDF_DECOMPOSER, ChupaPDFDecomposer)
#define CHUPA_PDF_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_PDF_DECOMPOSER, ChupaPDFDecomposerClass)
#define CHUPA_IS_PDF_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_PDF_DECOMPOSER)
#define CHUPA_IS_PDF_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_PDF_DECOMPOSER)
#define CHUPA_PDF_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_PDF_DECOMPOSER, ChupaPDFDecomposerClass)

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

static ChupaTextInput *
chupa_pdf_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
                          ChupaTextInput *input, GError **err)
{
    PopplerDocument *doc;
    GMemoryInputStream *mem = NULL;
    gssize count;
    gchar buffer[16*1024];
    const gsize bufsize = sizeof(buffer);
    GString *str = g_string_sized_new(sizeof(buffer));
    GInputStream *inp = chupa_text_input_get_stream(input);
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    ChupaTextInput *pdf_text = NULL;
    int n, i;

    while ((count = g_input_stream_read(inp, buffer, bufsize, NULL, err)) > 0) {
        g_string_append_len(str, buffer, count);
        if (count < bufsize) break;
    }
    if (count < 0) {
        g_string_free(str, TRUE);
        return FALSE;
    }
    doc = poppler_document_new_from_data(str->str, str->len, NULL, err);
    if (!doc) {
        g_string_free(str, TRUE);
        return FALSE;
    }
    n = poppler_document_get_n_pages(doc);
    for (i = 0; i < n; ++i) {
        PopplerPage *page = poppler_document_get_page(doc, i);
        gchar *text;
#if POPPLER_CHECK_VERSION(0, 14, 0)
        text = poppler_page_get_text(page);
#else
        PopplerRectangle rectangle = {0, 0, 0, 0};
        poppler_page_get_size (page, &rectangle.x2, &rectangle.y2);
        text = poppler_page_get_text(page, POPPLER_SELECTION_GLYPH, &rectangle);
#endif
        if (mem) {
            g_memory_input_stream_add_data(mem, "\f", 1, NULL);
            g_memory_input_stream_add_data(mem, text, -1, g_free);
        }
        else {
            GsfInput *base_input = chupa_text_input_get_base_input(input);
            const char *name = base_input ? gsf_input_name(base_input) : NULL;
            inp = g_memory_input_stream_new_from_data(text, -1, g_free);
            pdf_text = chupa_text_input_new_from_stream(meta, inp, name);
            chupa_text_decomposed(chupar, pdf_text);
            mem = (GMemoryInputStream *)inp;
        }
        g_object_unref(page);
    }
    g_object_unref(doc);
    g_string_free(str, TRUE);
    g_object_unref(mem);
    return pdf_text;
}

static void
chupa_pdf_decomposer_class_init(ChupaPDFDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_pdf_decomposer_feed;
}


static void
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaPDFDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_pdf_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaPDFDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };

    chupa_type_pdf_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    "ChupaPDFDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    registered_types =
        g_list_prepend(registered_types,
                       (gchar *)g_type_name(chupa_type_pdf_decomposer));

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_PDF_DECOMPOSER,
                               first_property, var_args);
}
