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

#include <chupatext/external_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_decomposer_factory.h>
#include <chupatext/chupa_text_input_stream.h>
#include <glib.h>
#include <gio/gio.h>
#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>
#include <gsf/gsf-output-memory.h>
#include <gsf/gsf-infile-msole.h>
#include <glib/gstdio.h>
#include <memory.h>
#include <unistd.h>

#define CHUPA_TYPE_POWER_POINT_DECOMPOSER            chupa_type_power_point_decomposer
#define CHUPA_POWER_POINT_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_POWER_POINT_DECOMPOSER, ChupaPowerPointDecomposer)
#define CHUPA_POWER_POINT_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_POWER_POINT_DECOMPOSER, ChupaPowerPointDecomposerClass)
#define CHUPA_IS_POWER_POINT_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_POWER_POINT_DECOMPOSER)
#define CHUPA_IS_POWER_POINT_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_POWER_POINT_DECOMPOSER)
#define CHUPA_POWER_POINT_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_POWER_POINT_DECOMPOSER, ChupaPowerPointDecomposerClass)

typedef struct _ChupaPowerPointDecomposer ChupaPowerPointDecomposer;
typedef struct _ChupaPowerPointDecomposerClass ChupaPowerPointDecomposerClass;

struct _ChupaPowerPointDecomposer
{
    ChupaExternalDecomposer parent_object;
};

struct _ChupaPowerPointDecomposerClass
{
    ChupaExternalDecomposerClass parent_class;
};

static GType chupa_type_power_point_decomposer = 0;

#define TMPFILE_BASE "chupa-XXXXXX"
static gboolean
feed(ChupaDecomposer *decomposer, ChupaText *chupar,
     ChupaTextInput *input, GError **error)
{
    const char *filename = chupa_text_input_get_filename(input);
    gchar *tmp_power_point_name, *tmp_pdf_name;
    gint fd_ppt, fd_pdf;
    GOutputStream *out_tmpfile;
    GInputStream *in_tmpfile;
    int argc;
    char *argv[5];
    gsize size;
    gboolean result;
    GsfInput *base_input;
    gint ooo_status;
    struct stat st;

    fd_ppt = g_file_open_tmp(TMPFILE_BASE".ppt", &tmp_power_point_name, error);
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
    g_return_val_if_fail(size != -1, (g_unlink(tmp_power_point_name), g_free(tmp_power_point_name), FALSE));
    fd_pdf = g_file_open_tmp(TMPFILE_BASE".pdf", &tmp_pdf_name, error);
    argc = 0;
    argv[argc++] = "ooffice";
    argv[argc++] = "-headless";
    argv[argc++] = tmp_power_point_name;
    argv[argc++] = g_strdup_printf("macro:///Standard.Export.WritePDF(\"file://%s\")", tmp_pdf_name);
    argv[argc++] = NULL;
    if (argc > (int)(sizeof(argv)/sizeof(argv[0]))) {
        g_error("argv overflow");
        return FALSE;
    }

    result = g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH,
                          NULL, NULL, NULL, NULL, &ooo_status, error);
    g_return_val_if_fail(result, FALSE);
    g_return_val_if_fail(ooo_status == 0, FALSE);

    sleep(2);                   /* Suck! */
    g_unlink(tmp_power_point_name);
    g_free(tmp_power_point_name);
    g_free(argv[3]);
    g_unlink(tmp_pdf_name);
    g_free(tmp_pdf_name);
    if (fstat(fd_pdf, &st) || st.st_size == 0) {
        close(fd_pdf);
        return FALSE;
    }
    in_tmpfile = g_unix_input_stream_new(fd_pdf, TRUE);
    input = chupa_text_input_new_from_stream(NULL, in_tmpfile, filename);
    g_object_unref(in_tmpfile);
    chupa_text_input_set_mime_type(input, "application/pdf");
    result = chupa_text_feed(chupar, input, error);
    g_object_unref(input);
    return result;
}

static void
decomposer_class_init(ChupaPowerPointDecomposerClass *klass)
{
    ChupaDecomposerClass *decomposer_class;

    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);
    decomposer_class->feed = feed;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaPowerPointDecomposerClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaPowerPointDecomposer),
        0,
        (GInstanceInitFunc)NULL,
    };
    const gchar *type_name = "ChupaPowerPointDecomposer";

    chupa_type_power_point_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_EXTERNAL_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

/* ChupaPowerPointDecomposerFactory */
#define CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY \
    (chupa_type_power_point_decomposer_factory)
#define CHUPA_POWER_POINT_DECOMPOSER_FACTORY(obj)                       \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY, \
                                ChupaPowerPointDecomposerFactory))
#define CHUPA_POWER_POINT_DECOMPOSER_FACTORY_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY, \
                             ChupaPowerPointDecomposerFactoryClass))
#define CHUPA_IS_POWER_POINT_DECOMPOSER_FACTORY(obj)                    \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY))
#define CHUPA_IS_POWER_POINT_DECOMPOSER_FACTORY_CLASS(klass)            \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY))
#define CHUPA_POWER_POINT_DECOMPOSER_FACTORY_GET_CLASS(obj)             \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY, \
                               ChupaPowerPointDecomposerFactoryClass))

typedef struct _ChupaPowerPointDecomposerFactory ChupaPowerPointDecomposerFactory;
typedef struct _ChupaPowerPointDecomposerFactoryClass ChupaPowerPointDecomposerFactoryClass;

struct _ChupaPowerPointDecomposerFactory
{
    ChupaDecomposerFactory     object;
};

struct _ChupaPowerPointDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_power_point_decomposer_factory = 0;
static ChupaDecomposerFactoryClass *factory_parent_class;

static GList     *get_mime_types   (ChupaDecomposerFactory *factory);
static GObject   *create           (ChupaDecomposerFactory *factory,
                                    const gchar            *label);

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
            sizeof (ChupaPowerPointDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaPowerPointDecomposerFactory),
            0,
            (GInstanceInitFunc)NULL,
        };
    const gchar *type_name = "ChupaPowerPointDecomposerFactory";

    chupa_type_power_point_decomposer_factory =
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

    mime_types = g_list_prepend(mime_types, g_strdup("text/plain"));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label)
{
    return g_object_new(CHUPA_TYPE_POWER_POINT_DECOMPOSER, NULL);
}

/* module entry points */
G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_CREATE_FACTORY(const gchar *first_property, va_list va_args)
{
    return g_object_new_valist(CHUPA_TYPE_POWER_POINT_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
