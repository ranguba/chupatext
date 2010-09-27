/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/external_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_text_input_stream.h>
#include <glib.h>
#include <gio/gio.h>
#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>
#include <gsf/gsf-output-memory.h>
#include <gsf/gsf-infile-msole.h>
#include <glib/gstdio.h>
#include <memory.h>

#define CHUPA_TYPE_PPT_DECOMPOSER            chupa_type_ppt_decomposer
#define CHUPA_PPT_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_PPT_DECOMPOSER, ChupaPPTDecomposer)
#define CHUPA_PPT_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_PPT_DECOMPOSER, ChupaPPTDecomposerClass)
#define CHUPA_IS_PPT_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_PPT_DECOMPOSER)
#define CHUPA_IS_PPT_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_PPT_DECOMPOSER)
#define CHUPA_PPT_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_PPT_DECOMPOSER, ChupaPPTDecomposerClass)

typedef struct _ChupaPPTDecomposer ChupaPPTDecomposer;
typedef struct _ChupaPPTDecomposerClass ChupaPPTDecomposerClass;

struct _ChupaPPTDecomposer
{
    ChupaExternalDecomposer parent_object;
};

struct _ChupaPPTDecomposerClass
{
    ChupaExternalDecomposerClass parent_class;
};

static GType chupa_type_ppt_decomposer = 0;

#define TMPFILE_BASE "chupa-XXXXXX"
static gboolean
chupa_feed_ppt(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInput *input, GError **error)
{
    const char *filename = chupa_text_input_get_filename(input);
    gchar *tmp_ppt_name, *tmp_pdf_name;
    gint fd_ppt, fd_pdf;
    GOutputStream *out_tmpfile;
    GInputStream *in_tmpfile;
    char *argv[5];
    gsize size;
    gboolean result;
    GsfInput *base_input;

    fd_ppt = g_file_open_tmp(TMPFILE_BASE".ppt", &tmp_ppt_name, error);
    out_tmpfile = g_unix_output_stream_new(fd_ppt, TRUE);
    base_input = chupa_text_input_get_base_input(input);
    gsf_input_seek(base_input, 0, G_SEEK_SET);
    in_tmpfile = chupa_text_input_get_stream(input);
    in_tmpfile = g_filter_input_stream_get_base_stream(G_FILTER_INPUT_STREAM(in_tmpfile));
    size = g_output_stream_splice(out_tmpfile, in_tmpfile,
                                  G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE|
                                  G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
                                  NULL, error);
    g_object_unref(out_tmpfile);
    g_return_val_if_fail(size != -1, (g_unlink(tmp_ppt_name), g_free(tmp_ppt_name), FALSE));
    fd_pdf = g_file_open_tmp(TMPFILE_BASE".pdf", &tmp_pdf_name, error);
    argv[0] = "ooffice";
    argv[1] = "-headless";
    argv[2] = tmp_ppt_name;
    argv[3] = g_strdup_printf("macro:///Standard.Export.WritePDF(\"file://%s\")", tmp_pdf_name);
    argv[4] = NULL;
    g_return_val_if_fail(chupa_external_decomposer_spawn(CHUPA_EXTERNAL_DECOMPOSER(dec),
                                                         argv, NULL, NULL, error),
                         FALSE);
    g_unlink(tmp_ppt_name);
    g_free(tmp_ppt_name);
    g_free(argv[3]);
    g_unlink(tmp_pdf_name);
    g_free(tmp_pdf_name);
    in_tmpfile = g_unix_input_stream_new(fd_pdf, TRUE);
    input = chupa_text_input_new_from_stream(NULL, in_tmpfile, filename);
    g_object_unref(in_tmpfile);
    chupa_metadata_replace_value(chupa_text_input_get_metadata(input),
                                 "mime-type", "application/pdf");
    result = chupa_text_feed(chupar, input, error);
    g_object_unref(input);
    return result;
}

static void
chupa_ppt_decomposer_class_init(ChupaPPTDecomposerClass *klass)
{
    ChupaDecomposerClass *dec_class = CHUPA_DECOMPOSER_CLASS(klass);
    dec_class->feed = chupa_feed_ppt;
}

static void
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaPPTDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_ppt_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaPPTDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };

    chupa_type_ppt_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_EXTERNAL_DECOMPOSER,
                                    "ChupaPPTDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    registered_types =
        g_list_prepend(registered_types,
                       (gchar *)g_type_name(chupa_type_ppt_decomposer));

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_PPT_DECOMPOSER,
                               first_property, var_args);
}
