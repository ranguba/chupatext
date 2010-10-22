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
#include <glib.h>

#include <chupatext/chupa_decomposer_module.h>
#include <chupatext/external_decomposer.h>
#include <chupatext/chupa_logger.h>
#include <chupatext/chupa_memory_input_stream.h>
#include <chupatext/chupa_data_input.h>
#include <goffice/goffice.h>
#include <gsf/gsf-output-memory.h>
#include "excel/workbook-view.h"
#include "excel/command-context-stderr.h"

static const gchar EXCEL_MIME_TYPE[] = "application/vnd.ms-excel";
static const gchar OFFICE_OPEN_XML_WORKBOOK_MIME_TYPE[] =               \
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";

/* ChupaExcelDecomposer */
#define CHUPA_TYPE_EXCEL_DECOMPOSER             \
    (chupa_type_excel_decomposer)
#define CHUPA_EXCEL_DECOMPOSER(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                CHUPA_TYPE_EXCEL_DECOMPOSER,    \
                                ChupaExcelDecomposer))
#define CHUPA_EXCEL_DECOMPOSER_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             CHUPA_TYPE_EXCEL_DECOMPOSER,       \
                             ChupaExcelDecomposerClass))
#define CHUPA_IS_EXCEL_DECOMPOSER(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                CHUPA_TYPE_EXCEL_DECOMPOSER))
#define CHUPA_IS_EXCEL_DECOMPOSER_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             CHUPA_TYPE_EXCEL_DECOMPOSER)
#define CHUPA_EXCEL_DECOMPOSER_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               CHUPA_TYPE_EXCEL_DECOMPOSER,     \
                               ChupaExcelDecomposerClass)

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

static const char excel_magic[8] = "\320\317\021\340\241\261\032\341";

static gboolean
chupa_excel_plain_file_p(GsfInput *source)
{
    guint8 header[sizeof(excel_magic)];
    if (!gsf_input_read(source, sizeof(header), header)) return FALSE;
    return memcmp(header, excel_magic, sizeof(header)) == 0;
}

static gboolean
feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
     ChupaData *data, GError **error)
{
    GOFileSaver *fs = NULL;
    GOFileOpener *fo = NULL;
    GOIOContext *io_context = go_io_context_new(cc);
    WorkbookView *wbv = NULL;
    GsfInput *source;
    GsfOutput *tmpout;
    GInputStream *tmpinp;
    ChupaData *next_data;
    const char *filename = chupa_data_get_filename(data);
    GPrintFunc old_print_error_func;
    ChupaMetadata *metadata;

    fs = go_file_saver_for_id(export_id);
    g_return_val_if_fail(fs, FALSE);
    source = chupa_data_input_new(data);
    g_return_val_if_fail(!gsf_input_seek(source, 0, G_SEEK_SET), FALSE);
    if (chupa_utils_string_equal(chupa_decomposer_get_mime_type(decomposer),
                                 EXCEL_MIME_TYPE) &&
        !chupa_excel_plain_file_p(source)) {
        /* encrypted file, skip */
        return FALSE;
    }
    g_return_val_if_fail(!gsf_input_seek(source, 0, G_SEEK_SET), FALSE);
    tmpout = gsf_output_memory_new();
    g_return_val_if_fail(tmpout, FALSE);
    old_print_error_func = g_set_printerr_handler(printerr_to_log_delegator);
    wbv = wb_view_new_from_input(source, filename, fo, io_context, NULL);
    g_object_unref(source);
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

    tmpinp = g_memory_input_stream_new_from_data(
        g_memdup(gsf_output_memory_get_bytes(GSF_OUTPUT_MEMORY(tmpout)),
                 gsf_output_size(tmpout)),
        gsf_output_size(tmpout),
        g_free);
    g_object_unref(io_context);
    g_object_unref(tmpout);
    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "filename", filename);
    next_data = chupa_data_new(tmpinp, metadata);
    g_object_unref(metadata);
    g_object_unref(tmpinp);
    chupa_feeder_accepted(feeder, next_data);
    chupa_data_finished(next_data, NULL);
    g_object_unref(next_data);

    return TRUE;
}

static void
decomposer_class_init(ChupaExcelDecomposerClass *klass)
{
    ChupaDecomposerClass *decomposer_class;

    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);
    decomposer_class->feed = feed;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaExcelDecomposerClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaExcelDecomposer),
        0,
        (GInstanceInitFunc)NULL,
    };
    const gchar *type_name = "ChupaExcelDecomposer";

    chupa_type_excel_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_EXTERNAL_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

/* ChupaExcelDecomposerFactory */
#define CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY \
    (chupa_type_excel_decomposer_factory)
#define CHUPA_EXCEL_DECOMPOSER_FACTORY(obj)                             \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY,    \
                                ChupaExcelDecomposerFactory))
#define CHUPA_EXCEL_DECOMPOSER_FACTORY_CLASS(klass)                     \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY,       \
                             ChupaExcelDecomposerFactoryClass))
#define CHUPA_IS_EXCEL_DECOMPOSER_FACTORY(obj)                          \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY))
#define CHUPA_IS_EXCEL_DECOMPOSER_FACTORY_CLASS(klass)                  \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY))
#define CHUPA_EXCEL_DECOMPOSER_FACTORY_GET_CLASS(obj)                   \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY,     \
                               ChupaExcelDecomposerFactoryClass))

typedef struct _ChupaExcelDecomposerFactory ChupaExcelDecomposerFactory;
typedef struct _ChupaExcelDecomposerFactoryClass ChupaExcelDecomposerFactoryClass;

struct _ChupaExcelDecomposerFactory
{
    ChupaDecomposerFactory     object;
};

struct _ChupaExcelDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_excel_decomposer_factory = 0;
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
            sizeof (ChupaExcelDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaExcelDecomposerFactory),
            0,
            (GInstanceInitFunc)NULL,
        };
    const gchar *type_name = "ChupaExcelDecomposerFactory";

    chupa_type_excel_decomposer_factory =
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

    mime_types = g_list_prepend(mime_types,
                                g_strdup(EXCEL_MIME_TYPE));
    mime_types = g_list_prepend(mime_types,
                                g_strdup(OFFICE_OPEN_XML_WORKBOOK_MIME_TYPE));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label, const gchar *mime_type)
{
    return g_object_new(CHUPA_TYPE_EXCEL_DECOMPOSER,
                        "mime-type", mime_type,
                        NULL);
}

/* module entry points */
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
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module)
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

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_DECOMPOSER_QUIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_DECOMPOSER_CREATE_FACTORY(const gchar *first_property, va_list va_args)
{
    return g_object_new_valist(CHUPA_TYPE_EXCEL_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
