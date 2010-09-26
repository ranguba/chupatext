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
#include "chupatext/chupa_gsf_input_stream.h"

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

static const char export_id[] = "Gnumeric_stf:stf_csv";

static void
silent_printerr(const gchar *string)
{
}

static void
silent_log(const gchar *log_domain, GLogLevelFlags log_level,
           const gchar *message, gpointer user_data)
{
}

static gboolean
chupa_excel_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
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
    GPrintFunc print_func;
    guint loghandler;

    fs = go_file_saver_for_id(export_id);
    g_return_val_if_fail(fs, FALSE);
    tmpout = gsf_output_memory_new();
    g_return_val_if_fail(tmpout, FALSE);
    g_return_val_if_fail(!gsf_input_seek(source, 0, G_SEEK_SET), FALSE);
    print_func = g_set_printerr_handler(silent_printerr);
    loghandler = g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, silent_log, NULL);
    wbv = wb_view_new_from_input(source, filename, fo, io_context, NULL);
    g_log_remove_handler(G_LOG_DOMAIN, loghandler);
    g_set_printerr_handler(print_func);
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

    tmpinp = chupa_gsf_input_stream_new(GSF_OUTPUT_MEMORY(tmpout));
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
    dec_class->feed = chupa_excel_decomposer_feed;
}

static void
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

    chupa_type_excel_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_EXTERNAL_DECOMPOSER,
                                    "ChupaExcelDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;
    GOErrorInfo	*plugin_errs;
    char *argv[2];

    argv[0] = NULL;
    gnm_pre_parse_init(0, argv);

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

    register_type(type_module);
    registered_types =
        g_list_prepend(registered_types,
                       (gchar *)g_type_name(chupa_type_excel_decomposer));

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
