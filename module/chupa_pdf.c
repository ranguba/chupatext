/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_decomposer.h>
#include <glib.h>
#include <glib/poppler.h>

#define CHUPA_TYPE_PDF_DECOMPOSER            chupa_pdf_decomposer_get_type()
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

G_DEFINE_TYPE(ChupaPDFDecomposer, chupa_pdf_decomposer, CHUPA_TYPE_DECOMPOSER)

static void
chupa_pdf_decomposer_init(ChupaPDFDecomposer *pdf_decomposer)
{
}

static void
chupa_pdf_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInputStream *stream)
{
    PopplerDocument *doc;
    GError **error = NULL;
    GMemoryInputStream *mem = NULL;
    gssize count;
    gchar buffer[16*1024];
    const gsize bufsize = sizeof(buffer);
    GString *str = g_string_sized_new(sizeof(buffer));
    GInputStream *inp = G_INPUT_STREAM(stream);
    ChupaMetadata *meta = chupa_text_input_stream_get_metadata(stream);
    int n, i;

    while ((count = g_input_stream_read(inp, buffer, bufsize, NULL, NULL)) > 0) {
        g_string_append_len(str, buffer, count);
        if (count < bufsize) break;
    }
    if (count < 0) {
        g_string_free(str, TRUE);
        return;
    }
    doc = poppler_document_new_from_data(str->str, str->len, NULL, NULL);
    if (!doc) {
        g_string_free(str, TRUE);
        return;
    }
    n = poppler_document_get_n_pages(doc);
    for (i = 0; i < n; ++i) {
        PopplerPage *page = poppler_document_get_page(doc, i);
#if POPPLER_CHECK_VERSION(0, 14, 0)
        char *text = poppler_page_get_text(page);
#else
        PopplerRectangle rectangle = {0, 0, 0, 0};
        poppler_page_get_size (page, &rectangle.x2, &rectangle.y2);
        char *text = poppler_page_get_text(page, POPPLER_SELECTION_GLYPH, &rectangle);
#endif
        if (mem) {
            g_memory_input_stream_add_data(mem, "\f", 1, NULL);
            g_memory_input_stream_add_data(mem, text, -1, g_free);
        }
        else {
            inp = g_memory_input_stream_new_from_data(text, -1, g_free);
            chupa_text_decomposed(chupar, chupa_text_input_stream_new(meta, inp));
            mem = (GMemoryInputStream *)inp;
        }
        g_object_unref(page);
    }
    g_object_unref(doc);
    g_string_free(str, TRUE);
    g_object_unref(mem);
}

static void
chupa_pdf_decomposer_class_init(ChupaPDFDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_pdf_decomposer_feed;
}
