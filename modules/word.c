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

#include <chupatext/chupa_decomposer_module.h>
#include <chupatext/chupa_data_input.h>

#include <ctype.h>
#include <glib.h>
#include <wv.h>

/* ChupaWordDecomposer */
#define CHUPA_TYPE_WORD_DECOMPOSER \
    (chupa_type_word_decomposer)
#define CHUPA_WORD_DECOMPOSER(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                CHUPA_TYPE_WORD_DECOMPOSER,     \
                                ChupaWordDecomposer)
#define CHUPA_WORD_DECOMPOSER_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             CHUPA_TYPE_WORD_DECOMPOSER,        \
                             ChupaWordDecomposerClass)
#define CHUPA_IS_WORD_DECOMPOSER(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                CHUPA_TYPE_WORD_DECOMPOSER)
#define CHUPA_IS_WORD_DECOMPOSER_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             CHUPA_TYPE_WORD_DECOMPOSER)
#define CHUPA_WORD_DECOMPOSER_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               CHUPA_TYPE_WORD_DECOMPOSER,      \
                               ChupaWordDecomposerClass)

typedef struct _ChupaWordDecomposer ChupaWordDecomposer;
typedef struct _ChupaWordDecomposerClass ChupaWordDecomposerClass;

struct _ChupaWordDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaWordDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_word_decomposer = 0;

struct char_proc_arg {
    GString *buffer;
    GMemoryInputStream *dest;
    ChupaFeeder *feeder;
    ChupaData *text;
    const char *encoding;
};

static int
char_proc(wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid)
{
    struct char_proc_arg *arg = ps->userData;
    GString *s = arg->buffer;

    if (!arg->encoding) {
        ChupaMetadata *meta = chupa_data_get_metadata(arg->text);
        if (chartype) {
            arg->encoding = wvLIDToCodePageConverter(lid);
        }
        else {
            arg->encoding = "UTF-8";
        }
        chupa_metadata_add_value(meta, "charset", arg->encoding);
    }

    /* take care of any oddities in Microsoft's character "encoding" */
    /* TODO: does the above code page handler take care of these? */
    if (chartype == 1 && eachchar == 146)
	eachchar = 39;		/* apostrophe */

    switch (eachchar) {
    case 13:			/* paragraph end */
    case 11:			/* hard line break */
        eachchar = '\n';
        break;

    case 12:			/* page breaks, section marks */
        eachchar = '\f';
        break;

    case 14:			/* column break */
        eachchar = '\t';
        break;

    case 19:			/* field begin */
        /* flush current text buffer */
        ps->fieldstate++;
        ps->fieldmiddle = 0;
        return 0;
    case 20:			/* field separator */
        ps->fieldmiddle = 1;
        return 0;
    case 21:			/* field end */
        ps->fieldstate--;
        ps->fieldmiddle = 0;
        return 0;

    default:
        if (eachchar < 256 && iscntrl(eachchar)) {
            eachchar = ' ';
        }
        break;
    }

    /* todo: properly handle fields */
    if (eachchar == 0x13 || eachchar == 0x14)
	return 0;

    if (!s) {
        s = g_string_new(0);
        arg->buffer = s;
    }

    /* convert incoming character to unicode */
    if (chartype) {
        g_string_append_c(s, eachchar);
    }
    else {
        g_string_append_unichar(s, eachchar);
    }

    if (eachchar == '\f') {
        GMemoryInputStream *stream;
        stream = G_MEMORY_INPUT_STREAM(arg->dest);
        g_memory_input_stream_add_data(stream, s->str, s->len, g_free);
        g_string_free(s, FALSE);
        arg->buffer = NULL;
        if (arg->feeder) {
            chupa_feeder_accepted(arg->feeder, arg->text);
        }
    }
    return 0;
}

static gboolean
feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
     ChupaData *data, GError **error)
{
    struct char_proc_arg arg;
    wvParseStruct ps;
    GsfInput *input;
    ChupaMetadata *metadata;
    int ret;

    arg.buffer = NULL;
    arg.dest = G_MEMORY_INPUT_STREAM(g_memory_input_stream_new());
    arg.feeder = feeder;
    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "filename",
                             chupa_data_get_filename(data));
    arg.text = chupa_data_new(G_INPUT_STREAM(arg.dest), metadata);
    g_object_unref(metadata);
    arg.encoding = NULL;

    input = chupa_data_input_new(data);
    if ((ret = wvInitParser_gsf(&ps, input)) != 0) {
        g_set_error(error,
                    CHUPA_FEEDER_ERROR,
                    CHUPA_FEEDER_ERROR_INVALID_INPUT,
                    "wvInitParser_gsf failed: %d",
                    ret);
        return FALSE;
    }
    ps.userData = &arg;
    wvSetCharHandler(&ps, char_proc);
    wvText(&ps);
    wvOLEFree(&ps);
    if (arg.buffer) {
        GString *s = arg.buffer;
        g_memory_input_stream_add_data(arg.dest, s->str, s->len, g_free);
        g_string_free(s, FALSE);
        arg.buffer = NULL;
        if (!arg.encoding) {
            ChupaMetadata *meta = chupa_data_get_metadata(arg.text);
            chupa_metadata_add_value(meta, "charset", "US-ASCII");
        }
        if (arg.feeder) {
            chupa_feeder_accepted(feeder, arg.text);
        }
    }
    chupa_data_finished(arg.text, NULL);
    g_object_unref(arg.dest);
    g_object_unref(arg.text);

    return TRUE;
}

static void
decomposer_class_init(ChupaWordDecomposerClass *klass)
{
    ChupaDecomposerClass *decomposer_class;

    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);
    decomposer_class->feed = feed;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaWordDecomposerClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaWordDecomposer),
        0,
        (GInstanceInitFunc)NULL,
    };
    const gchar *type_name = "ChupaWordDecomposer";

    chupa_type_word_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

/* ChupaWordDecomposerFactory */
#define CHUPA_TYPE_WORD_DECOMPOSER_FACTORY \
    (chupa_type_word_decomposer_factory)
#define CHUPA_WORD_DECOMPOSER_FACTORY(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_WORD_DECOMPOSER_FACTORY,     \
                                ChupaWordDecomposerFactory))
#define CHUPA_WORD_DECOMPOSER_FACTORY_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             CHUPA_TYPE_WORD_DECOMPOSER_FACTORY,        \
                             ChupaWordDecomposerFactoryClass))
#define CHUPA_IS_WORD_DECOMPOSER_FACTORY(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_WORD_DECOMPOSER_FACTORY))
#define CHUPA_IS_WORD_DECOMPOSER_FACTORY_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_WORD_DECOMPOSER_FACTORY))
#define CHUPA_WORD_DECOMPOSER_FACTORY_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_WORD_DECOMPOSER_FACTORY,      \
                               ChupaWordDecomposerFactoryClass))

typedef struct _ChupaWordDecomposerFactory ChupaWordDecomposerFactory;
typedef struct _ChupaWordDecomposerFactoryClass ChupaWordDecomposerFactoryClass;

struct _ChupaWordDecomposerFactory
{
    ChupaDecomposerFactory     object;
};

struct _ChupaWordDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_word_decomposer_factory = 0;
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
            sizeof (ChupaWordDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaWordDecomposerFactory),
            0,
            (GInstanceInitFunc)NULL,
        };
    const gchar *type_name = "ChupaWordDecomposerFactory";

    chupa_type_word_decomposer_factory =
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

    mime_types = g_list_prepend(mime_types, g_strdup("application/msword"));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label, const gchar *mime_type)
{
    return g_object_new(CHUPA_TYPE_WORD_DECOMPOSER,
                        "mime-type", mime_type,
                        NULL);
}

/* module entry points */
G_MODULE_EXPORT GList *
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module, GError **error)
{
    GList *registered_types = NULL;

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    return registered_types;
}

G_MODULE_EXPORT gboolean
CHUPA_DECOMPOSER_QUIT(void)
{
    return TRUE;
}

G_MODULE_EXPORT GObject *
CHUPA_DECOMPOSER_CREATE_FACTORY(const gchar *first_property, va_list va_args)
{
    return g_object_new_valist(CHUPA_TYPE_WORD_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
