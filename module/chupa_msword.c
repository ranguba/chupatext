/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_restrict_input_stream.h>
#include <glib.h>
#include <wv.h>

#define CHUPA_TYPE_MSWORD_DECOMPOSER chupa_type_msword_decomposer
#define CHUPA_MSWORD_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_MSWORD_DECOMPOSER, ChupaMSWORDDecomposer)
#define CHUPA_MSWORD_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_MSWORD_DECOMPOSER, ChupaMSWORDDecomposerClass)
#define CHUPA_IS_MSWORD_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_MSWORD_DECOMPOSER)
#define CHUPA_IS_MSWORD_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_MSWORD_DECOMPOSER)
#define CHUPA_MSWORD_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_MSWORD_DECOMPOSER, ChupaMSWORDDecomposerClass)

typedef struct _ChupaMSWORDDecomposer ChupaMSWORDDecomposer;
typedef struct _ChupaMSWORDDecomposerClass ChupaMSWORDDecomposerClass;

struct _ChupaMSWORDDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaMSWORDDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_msword_decomposer = 0;

struct char_proc_arg {
    GString *buffer;
    GMemoryInputStream *dest;
    ChupaText *chupar;
    ChupaTextInput *input;
    const char *encoding;
};

static int
char_proc(wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid)
{
    struct char_proc_arg *arg = ps->userData;
    GString *s = arg->buffer;

    if (!arg->encoding) {
        ChupaMetadata *meta = chupa_text_input_get_metadata(arg->input);
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
        if (arg->chupar) {
            chupa_text_decomposed(arg->chupar, arg->input);
        }
    }
    return 0;
}

static gboolean
chupa_msword_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
                             ChupaTextInput *input, GError **err)
{
    struct char_proc_arg arg;
    wvParseStruct ps;
    GsfInput *gi = chupa_text_input_get_base_input(input);
    int ret;

    arg.buffer = NULL;
    arg.dest = G_MEMORY_INPUT_STREAM(g_memory_input_stream_new());
    arg.chupar = chupar;
    arg.input = chupa_text_input_new_from_stream(NULL, G_INPUT_STREAM(arg.dest),
                                                 gsf_input_name(gi));
    arg.encoding = NULL;

    gsf_input_seek(gi, 0, G_SEEK_SET);
    if ((ret = wvInitParser_gsf(&ps, gi)) != 0) {
        g_propagate_error(err, chupa_text_error_new(CHUPA_TEXT_ERROR_INVALID_INPUT,
                                                    "wvInitParser_gsf failed: %d",
                                                    ret));
        return;
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
            ChupaMetadata *meta = chupa_text_input_get_metadata(arg.input);
            chupa_metadata_add_value(meta, "charset", "US-ASCII");
        }
        if (arg.chupar) {
            chupa_text_decomposed(chupar, arg.input);
        }
    }
    g_object_unref(arg.dest);
    g_object_unref(arg.input);
}

static void
chupa_msword_decomposer_class_init(ChupaMSWORDDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_msword_decomposer_feed;
}

static GType
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaMSWORDDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_msword_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaMSWORDDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };
    GType type = chupa_type_msword_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_DECOMPOSER,
                                           "ChupaMSWORDDecomposer",
                                           &info, 0);
        chupa_type_msword_decomposer = type;
        chupa_decomposer_register("application/msword", type);
    }
    return type;
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GType type = register_type(type_module);
    GList *registered_types = NULL;

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
    return g_object_new_valist(CHUPA_TYPE_MSWORD_DECOMPOSER,
                               first_property, var_args);
}
