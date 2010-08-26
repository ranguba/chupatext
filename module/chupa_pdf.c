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
chupa_pdf_decomposer_feed(ChupaDecomposer *dec, ChupaText *text, ChupaTextInputStream *stream)
{
    PopplerDocument *doc;
    GError **error = NULL;
    gchar *tempfile = save_to_tempfile(stream, error);
    GMemoryInputStream *mem = NULL;
    const gsize bufsize = 16*1024;
    gsize len = 0;
    gssize count;
    char *buffer, *tmp;
    GInputStream *inp = G_INPUT_STREAM(stream);
    ChupaMetadata *meta = chupa_text_input_stream_get_metadata(stream);
    int n, i;

    if (!(buffer = g_malloc(bufsize))) {
        return;
    }
    while ((count = g_input_stream_read(stream, buffer + len, bufsize, NULL, NULL)) == bufsize) {
        len += bufsize;
        if (!(tmp = g_realloc(buffer, len + bufsize))) {
            g_free(buffer);
            return;
        }
        buffer = tmp;
    }
    if (count < 0) {
        g_free(buffer);
        return;
    }
    len += count;
    doc = poppler_document_new_from_data(buffer, len, NULL, NULL);
    if (!doc) {
        g_free(buffer);
        return;
    }
    n = poppler_document_get_n_pages(doc);
    for (i = 0; i < n; ++i) {
        PopplerPage *page = poppler_document_get_page(doc, i);
        char *text = poppler_page_get_text(page);
        if (mem) {
            g_memory_input_stream_add_data(mem, "\f", 1, NULL);
            g_memory_input_stream_add_data(mem, text, -1, g_free);
        }
        else {
            inp = g_memory_input_stream_new_from_data(text, -1, g_free);
            chupa_text_decomposed(text, chupa_text_input_stream_new(meta, inp));
            mem = (GMemoryInputStream *)inp;
        }
        g_object_unref(page);
    }
    g_object_unref(doc);
    g_object_unref(mem);
}

static void
chupa_pdf_decomposer_class_init(ChupaPDFDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_pdf_decomposer_feed;
}
