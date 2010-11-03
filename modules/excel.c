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
#include <chupatext/chupa_logger.h>
#include <chupatext/chupa_memory_input_stream.h>
#include <chupatext/chupa_data_input.h>
#include <goffice/goffice.h>
#include <gsf/gsf-output-memory.h>
#include "excel/workbook-view.h"
#include "excel/libgnumeric.h"
#include "excel/gnumeric-gconf.h"
#include "excel/gnm-plugin.h"

static const gchar EXCEL_MIME_TYPE[] = "application/vnd.ms-excel";
static const gchar OFFICE_OPEN_XML_WORKBOOK_MIME_TYPE[] =               \
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";

/* ChupaExcelCommandContext */
#define CHUPA_TYPE_EXCEL_COMMAND_CONTEXT        \
    (chupa_type_excel_command_context)
#define CHUPA_EXCEL_COMMAND_CONTEXT(obj)                                \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_EXCEL_COMMAND_CONTEXT,       \
                                ChupaExcelCommandContext))
#define CHUPA_EXCEL_COMMAND_CONTEXT_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             CHUPA_TYPE_EXCEL_COMMAND_CONTEXT,  \
                             ChupaExcelCommandContextClass))
#define CHUPA_IS_EXCEL_COMMAND_CONTEXT(obj)                             \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_EXCEL_COMMAND_CONTEXT))
#define CHUPA_IS_EXCEL_COMMAND_CONTEXT_CLASS(klass)             \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             CHUPA_TYPE_EXCEL_COMMAND_CONTEXT)
#define CHUPA_EXCEL_COMMAND_CONTEXT_GET_CLASS(obj)                      \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_EXCEL_COMMAND_CONTEXT,        \
                               ChupaExcelCommandContextClass)

typedef struct _ChupaExcelCommandContext ChupaExcelCommandContext;
typedef struct _ChupaExcelCommandContextClass ChupaExcelCommandContextClass;

struct _ChupaExcelCommandContext
{
    GObject parent_object;
};

struct _ChupaExcelCommandContextClass
{
    GObjectClass parent_class;
};

static GType chupa_type_excel_command_context = 0;

static GOCmdContext *
command_context_new(void)
{
    return g_object_new(CHUPA_TYPE_EXCEL_COMMAND_CONTEXT, NULL);
}

static void
command_context_error_error(GOCmdContext *context, GError *error)
{
    chupa_log_g_error(error, "[excel][error]");
}

static void
command_context_report_goffice_error_info(GOErrorInfo *error, gint depth)
{
    GSList *node;
    const gchar *message;

    message = go_error_info_peek_message(error);
    if (message) {
        GOSeverity severity;

        severity = go_error_info_peek_severity(error);
        switch (severity) {
        case GO_WARNING:
            chupa_warning("%*s[excel][warning]: %s", depth, "", message);
            break;
        default:
            chupa_error("%*s[excel][error]: %s", depth, "", message);
            break;
        }
        depth++;
    }

    for (node = go_error_info_peek_details(error);
         node;
         node = g_slist_next(node)) {
        GOErrorInfo *error_detail = node->data;
        command_context_report_goffice_error_info(error_detail, depth);
    }
}

static void
command_context_error_info(GOCmdContext *context, GOErrorInfo *error)
{
    command_context_report_goffice_error_info(error, 0);
}

static gchar *
command_context_get_password(GOCmdContext *context, const gchar *filename)
{
    return NULL;
}

static void
command_context_set_sensitive(GOCmdContext *context, gboolean sensitive)
{
}

static void
command_context_progress_set(GOCmdContext *context, gdouble value)
{
}

static void
command_context_progress_message_set(GOCmdContext *context, const gchar *message)
{
}

static void
command_context_init(ChupaExcelCommandContext *context)
{
}

static void
command_context_interface_init(GOCmdContextClass *context_class)
{
    context_class->get_password         = command_context_get_password;
    context_class->set_sensitive        = command_context_set_sensitive;
    context_class->progress_set         = command_context_progress_set;
    context_class->progress_message_set = command_context_progress_message_set;
    context_class->error.error          = command_context_error_error;
    context_class->error.error_info     = command_context_error_info;
}

static void
command_context_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaExcelCommandContextClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)NULL,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaExcelCommandContext),
        0,
        (GInstanceInitFunc)command_context_init,
    };
    static GInterfaceInfo interface_info = {
        (GInterfaceInitFunc)command_context_interface_init,
        (GInterfaceFinalizeFunc)NULL,
        NULL /* interface data */
    };
    const gchar *type_name = "ChupaExcelCommandContext";

    chupa_type_excel_command_context =
        g_type_module_register_type(type_module,
                                    G_TYPE_OBJECT,
                                    type_name,
                                    &info, 0);
    g_type_module_add_interface(type_module,
                                chupa_type_excel_command_context,
                                GO_TYPE_CMD_CONTEXT,
                                &interface_info);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

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
    ChupaDecomposer parent_object;
};

struct _ChupaExcelDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GOCmdContext *command_context;
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
chupa_excel_encrypted_file_p(ChupaDecomposer *decomposer, GsfInput *source)
{
    if (!chupa_utils_string_equal(chupa_decomposer_get_mime_type(decomposer),
                                  EXCEL_MIME_TYPE))
        return FALSE;

    return !chupa_excel_plain_file_p(source);
}


static gboolean
feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
     ChupaData *data, GError **error)
{
    GOFileSaver *saver = NULL;
    GOFileOpener *opener = NULL;
    GOIOContext *io_context;
    WorkbookView *view = NULL;
    GsfInput *source;
    GsfOutput *output;
    GInputStream *input;
    ChupaData *next_data;
    const gchar *filename;
    GPrintFunc old_print_error_func;
    ChupaMetadata *metadata;

    filename = chupa_data_get_filename(data);

    source = chupa_data_input_new(data);
    if (chupa_excel_encrypted_file_p(decomposer, source)) {
        g_set_error(error,
                    CHUPA_DECOMPOSER_ERROR,
                    CHUPA_DECOMPOSER_ERROR_FEED,
                    "[excel][feed][%s][unsupported]: "
                    "encrypted Excel file isn't supported",
                    filename);
        g_object_unref(source);
        return FALSE;
    }

    if (gsf_input_seek(source, 0, G_SEEK_SET)) {
        g_set_error(error,
                    CHUPA_DECOMPOSER_ERROR,
                    CHUPA_DECOMPOSER_ERROR_FEED,
                    "[excel][feed][%s][error]: failed to seek input to head",
                    filename);
        g_object_unref(source);
        return FALSE;
    }

    io_context = go_io_context_new(command_context);
    old_print_error_func = g_set_printerr_handler(printerr_to_log_delegator);
    view = wb_view_new_from_input(source, filename, opener, io_context, NULL);
    g_object_unref(source);
    g_set_printerr_handler(old_print_error_func);
    if (go_io_error_occurred(io_context)) {
        go_io_error_display(io_context);
    }
    if (!view) {
        g_set_error(error,
                    CHUPA_DECOMPOSER_ERROR,
                    CHUPA_DECOMPOSER_ERROR_FEED,
                    "[excel][feed][%s][error]: failed to create workbook",
                    filename);
        g_object_unref(io_context);
        return FALSE;
    }

    saver = go_file_saver_for_id(export_id);
    if (!saver) {
        g_set_error(error,
                    CHUPA_DECOMPOSER_ERROR,
                    CHUPA_DECOMPOSER_ERROR_FEED,
                    "[excel][feed][%s][error]: "
                    "failed to create file saver: <%s>",
                    filename,
                    export_id);
        g_object_unref(view);
        return FALSE;
    }

    output = gsf_output_memory_new();
    if (!output) {
        g_set_error(error,
                    CHUPA_DECOMPOSER_ERROR,
                    CHUPA_DECOMPOSER_ERROR_FEED,
                    "[excel][feed][%s][error]: failed to create output",
                    filename);
        g_object_unref(view);
        return FALSE;
    }

    wbv_save_to_output(view, saver, output, io_context);
    g_object_unref(wb_view_get_workbook(view));
    if (go_io_error_occurred(io_context)) {
        go_io_error_display(io_context);
        g_object_unref(io_context);
        g_object_unref(output);
        return FALSE;
    }
    g_object_unref(io_context);

    input = chupa_memory_input_stream_new(GSF_OUTPUT_MEMORY(output));
    g_object_unref(output);

    metadata = chupa_metadata_new();
    chupa_metadata_set_string(metadata, "filename", filename);
    next_data = chupa_data_new(input, metadata);
    g_object_unref(metadata);
    g_object_unref(input);
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
                                    CHUPA_TYPE_DECOMPOSER,
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

G_MODULE_EXPORT GList *
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module, GError **error)
{
    GList *registered_types = NULL;
    GOErrorInfo	*plugin_errors = NULL;
    const gchar *argv[1];

    command_context_register_type(type_module, &registered_types);

    argv[0] = g_get_prgname();
    gnm_pre_parse_init(1, argv);
    gnm_init();
    command_context = command_context_new();
    gnm_plugins_init(GO_CMD_CONTEXT(command_context));
    go_plugin_db_activate_plugin_list(go_plugins_get_available_plugins(),
                                      &plugin_errors);
    if (plugin_errors) {
        g_set_error(error, CHUPA_DECOMPOSER_ERROR, CHUPA_DECOMPOSER_ERROR_INIT,
                    "failed to initialize GOffice plugins : %s",
                    go_error_info_peek_message(plugin_errors));
        go_error_info_free(plugin_errors);
    }

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    return registered_types;
}

G_MODULE_EXPORT gboolean
CHUPA_DECOMPOSER_QUIT(void)
{
    g_object_unref(command_context);
    gnm_shutdown();
    gnm_pre_parse_shutdown();

    return FALSE;
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
