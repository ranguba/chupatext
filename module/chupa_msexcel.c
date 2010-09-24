/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/external_decomposer.h>
#include <chupatext/chupa_module.h>
#include <goffice/goffice.h>
#include <gsf/gsf-output-memory.h>
#include <glib.h>
#include "workbook-view.h"
#include "command-context-stderr.h"

#define CHUPA_TYPE_EXCEL_DECOMPOSER            chupa_type_excel_decomposer
#define CHUPA_EXCEL_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_EXCEL_DECOMPOSER, ChupaExcelDecomposer)
#define CHUPA_EXCEL_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_EXCEL_DECOMPOSER, ChupaExcelDecomposerClass)
#define CHUPA_IS_EXCEL_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_EXCEL_DECOMPOSER)
#define CHUPA_IS_EXCEL_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_EXCEL_DECOMPOSER)
#define CHUPA_EXCEL_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_EXCEL_DECOMPOSER, ChupaExcelDecomposerClass)

typedef struct _ChupaExcelDecomposer ChupaExcelDecomposer;
typedef struct _ChupaExcelDecomposerClass ChupaExcelDecomposerClass;

struct _ChupaExcelDecomposer
{
    ChupaExternalDecomposer parent_object;
};

struct _ChupaExcelDecomposerClass
{
    ChupaExternalDecomposerClass parent_class;
};

static GOCmdContext *cc;
static GType chupa_type_excel_decomposer = 0;

#define CHUPA_TYPE_TUNNEL_STREAM            chupa_tunnel_stream_get_type()
#define CHUPA_TUNNEL_STREAM(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TUNNEL_STREAM, ChupaTunnelStream)
#define CHUPA_TUNNEL_STREAM_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TUNNEL_STREAM, ChupaTunnelStreamClass)
#define CHUPA_IS_TUNNEL_STREAM(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TUNNEL_STREAM)
#define CHUPA_IS_TUNNEL_STREAM_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TUNNEL_STREAM)
#define CHUPA_TUNNEL_STREAM_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TUNNEL_STREAM, ChupaTunnelStreamClass)

typedef struct _ChupaTunnelStream ChupaTunnelStream;
typedef struct _ChupaTunnelStreamClass ChupaTunnelStreamClass;

struct _ChupaTunnelStream
{
    GMemoryInputStream parent_object;
    GsfOutputMemory *source;
};

struct _ChupaTunnelStreamClass
{
    GMemoryInputStreamClass parent_class;
};

G_DEFINE_TYPE(ChupaTunnelStream, chupa_tunnel_stream, G_TYPE_MEMORY_INPUT_STREAM)

static void
tunnel_dispose(GObject *object)
{
    ChupaTunnelStream *stream = CHUPA_TUNNEL_STREAM(object);

    if (stream->source) {
        g_object_unref(stream->source);
        stream->source = NULL;
    }

    G_OBJECT_CLASS(chupa_tunnel_stream_parent_class)->dispose(object);
}

static void
chupa_tunnel_stream_init(ChupaTunnelStream *stream)
{
    stream->source = NULL;
}

static void
chupa_tunnel_stream_class_init(ChupaTunnelStreamClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose      = tunnel_dispose;
}

static GInputStream *
chupa_tunnel_stream_new(GsfOutputMemory *mem)
{
    ChupaTunnelStream *stream = g_object_new(CHUPA_TYPE_TUNNEL_STREAM, NULL);
    GsfOutput *out = GSF_OUTPUT(mem);

    g_object_ref(mem);
    stream->source = mem;
    g_memory_input_stream_add_data(G_MEMORY_INPUT_STREAM(stream),
                                   gsf_output_memory_get_bytes(mem),
                                   gsf_output_size(out), NULL);
    return G_INPUT_STREAM(stream);
}

static const char export_id[] = "Gnumeric_stf:stf_csv";

static gboolean
chupa_msword_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
                             ChupaTextInput *input, GError **err)
{
    ChupaExcelDecomposer *xlsdec = CHUPA_EXCEL_DECOMPOSER(dec);
    GOFileSaver *fs = NULL;
    GOFileOpener *fo = NULL;
    GOIOContext *io_context = go_io_context_new(cc);
    WorkbookView *wbv = NULL;
    GsfInput *source = chupa_text_input_get_base_input(input);
    GsfOutput *tmpout;
    GInputStream *tmpinp;
    const char *filename = chupa_text_input_get_filename(input);

    fs = go_file_saver_for_id(export_id);
    g_return_val_if_fail(fs, FALSE);
    tmpout = gsf_output_memory_new();
    g_return_val_if_fail(tmpout, FALSE);
    g_return_val_if_fail(!gsf_input_seek(source, 0, G_SEEK_SET), FALSE);
    fo = go_file_opener_for_id("Gnumeric_Excel:excel");
    wbv = wb_view_new_from_input(source, filename, fo, io_context, NULL);
    if (go_io_error_occurred(io_context)) {
        go_io_error_display(io_context);
    }
    g_return_val_if_fail(wbv, FALSE);

    wbv_save_to_output(wbv, fs, tmpout, io_context);
    g_object_unref(wbv);
    if (go_io_error_occurred(io_context)) {
        g_object_unref(io_context);
        g_object_unref(tmpout);
        return FALSE;
    }

    tmpinp = chupa_tunnel_stream_new(GSF_OUTPUT_MEMORY(tmpout));
    g_object_unref(io_context);
    g_object_unref(tmpout);
    input = chupa_text_input_new_from_stream(NULL, tmpinp, chupa_text_input_get_filename(input));
    g_object_unref(tmpinp);
    chupa_text_decomposed(chupar, input);
    return TRUE;
}

static void
chupa_excel_decomposer_init(ChupaExcelDecomposer *dec)
{
}

static void
chupa_excel_constructed(GObject *object)
{
    ChupaExcelDecomposer *dec = CHUPA_EXCEL_DECOMPOSER(object);
}

static void
chupa_excel_decomposer_class_init(ChupaExcelDecomposerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    ChupaDecomposerClass *dec_class = CHUPA_DECOMPOSER_CLASS(klass);
    gobject_class->constructed = chupa_excel_constructed;
    dec_class->feed = chupa_msword_decomposer_feed;
}

static GType
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaExcelDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_excel_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaExcelDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };
    GType type = chupa_type_excel_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_EXTERNAL_DECOMPOSER,
                                           "ChupaExcelDecomposer",
                                           &info, 0);
        chupa_type_excel_decomposer = type;
        chupa_decomposer_register("application/vnd.ms-excel", type);
    }
    return type;
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GType type = register_type(type_module);
    GList *registered_types = NULL;
    GOErrorInfo	*plugin_errs;

    gnm_init ();
    gnm_conf_init();
    cc = cmd_context_stderr_new();
    gnm_plugins_init(GO_CMD_CONTEXT(cc));
    go_plugin_db_activate_plugin_list(go_plugins_get_available_plugins(), &plugin_errs);
    if (plugin_errs) {
        fprintf(stderr, "go_plugin_db_activate_plugin_list failed\n");
        go_error_display(plugin_errs);
        go_error_info_free(plugin_errs);
    }

#if 0
    if (type) {
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(type));
    }
#endif

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_EXCEL_DECOMPOSER,
                               first_property, var_args);
}
