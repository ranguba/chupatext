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

#include <string.h>
#include <glib.h>

#include <chupatext/external_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_logger.h>
#include <goffice/goffice.h>
#include <gsf/gsf-output-memory.h>
#include "excel/workbook-view.h"
#include "excel/command-context-stderr.h"
#include "chupatext/chupa_memory_input_stream.h"

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
printerr_to_log_delegator (const gchar *string)
{
    gint length;

    length = strlen(string);
    if (string[length - 1] == '\n') {
        chupa_warning("%.*s", length - 1, string);
    } else {
        chupa_warning("%s", string);
    }
}

static gboolean
chupa_excel_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
                            ChupaTextInput *input, GError **err)
{
    GOFileSaver *fs = NULL;
    GOFileOpener *fo = NULL;
    GOIOContext *io_context = go_io_context_new(cc);
    WorkbookView *wbv = NULL;
    GsfInput *source = chupa_text_input_get_base_input(input);
    GsfOutput *tmpout;
    GInputStream *tmpinp;
    const char *filename = chupa_text_input_get_filename(input);
    GPrintFunc old_print_error_func;

    fs = go_file_saver_for_id(export_id);
    g_return_val_if_fail(fs, FALSE);
    tmpout = gsf_output_memory_new();
    g_return_val_if_fail(tmpout, FALSE);
    g_return_val_if_fail(!gsf_input_seek(source, 0, G_SEEK_SET), FALSE);
    old_print_error_func = g_set_printerr_handler(printerr_to_log_delegator);
    wbv = wb_view_new_from_input(source, filename, fo, io_context, NULL);
    g_set_printerr_handler(old_print_error_func);
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

    tmpinp = chupa_memory_input_stream_new(GSF_OUTPUT_MEMORY(tmpout));
    g_object_unref(io_context);
    g_object_unref(tmpout);
    input = chupa_text_input_new_from_stream(NULL, tmpinp, chupa_text_input_get_filename(input));
    g_object_unref(tmpinp);
    chupa_text_decomposed(chupar, input);
    return TRUE;
}

static void
chupa_excel_decomposer_class_init(ChupaExcelDecomposerClass *klass)
{
    ChupaDecomposerClass *dec_class = CHUPA_DECOMPOSER_CLASS(klass);
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

/* copied from libgnumeric.h */
char const **gnm_pre_parse_init     (int argc, gchar const **argv);
void	     gnm_pre_parse_shutdown (void);
void	     gnm_init		    (void);
void	     gnm_shutdown	    (void);
/* copied from gnumeric-gconf.h */
void     gnm_conf_init (void);
void     gnm_conf_shutdown (void);
GOConfNode *gnm_conf_get_root (void);
/* copied from gnm-plugin.h */
void gnm_plugins_init (GOCmdContext *c);

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    extern void gutils_init(void);
    GList *registered_types = NULL;
    GOErrorInfo	*plugin_errs;

    gutils_init();

    gnm_init ();
    gnm_conf_init();
    cc = cmd_context_stderr_new();
    gnm_plugins_init(GO_CMD_CONTEXT(cc));
    go_plugin_db_activate_plugin_list(go_plugins_get_available_plugins(), &plugin_errs);
    if (plugin_errs) {
        if (go_error_info_peek_severity(plugin_errs) >= GO_ERROR) {
            g_warning("go_plugin_db_activate_plugin_list failed: %s",
                      go_error_info_peek_message(plugin_errs));
        }
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
