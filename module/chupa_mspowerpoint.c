/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/archive_decomposer.h>
#include <chupatext/chupa_module.h>
#include <glib.h>
#include <gsf/gsf-infile-msole.h>

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
    ChupaArchiveDecomposer parent_object;
};

struct _ChupaPPTDecomposerClass
{
    ChupaArchiveDecomposerClass parent_class;
};

static GType chupa_type_ppt_decomposer = 0;

/* borrowed from catdoc-0.94.2 */
typedef enum {
    PPT_ITEM_DOCUMENT                      = 1000,
    PPT_ITEM_DOCUMENT_ATOM                 = 1001,
    PPT_ITEM_DOCUMENT_END                  = 1002,
    PPT_ITEM_SLIDE_PERSIST                 = 1003,
    PPT_ITEM_SLIDE_BASE                    = 1004,
    PPT_ITEM_SLIDE_BASE_ATOM               = 1005,
    PPT_ITEM_SLIDE                         = 1006,
    PPT_ITEM_SLIDE_ATOM                    = 1007,
    PPT_ITEM_NOTES                         = 1008 ,
    PPT_ITEM_NOTES_ATOM                    = 1009,
    PPT_ITEM_ENVIRONMENT                   = 1010,
    PPT_ITEM_SLIDE_PERSIST_ATOM            = 1011,
    PPT_ITEM_MAIN_MASTER                   = 1016,
    PPT_ITEM_SSSLIDE_INFO_ATOM             = 1017,
    PPT_ITEM_SSDOC_INFO_ATOM               = 1025,
    PPT_ITEM_EX_OBJ_LIST                   = 1033,
    PPT_ITEM_PPDRAWING_GROUP               = 1035,
    PPT_ITEM_PPDRAWING                     = 1036,
    PPT_ITEM_LIST                          = 2000,
    PPT_ITEM_COLOR_SCHEME_ATOM             = 2032,
    PPT_ITEM_TEXT_HEADER_ATOM              = 3999,
    PPT_ITEM_TEXT_CHARS_ATOM               = 4000,
    PPT_ITEM_STYLE_TEXT_PROP_ATOM          = 4001,
    PPT_ITEM_TX_MASTER_STYLE_ATOM          = 4003,
    PPT_ITEM_TEXT_BYTES_ATOM               = 4008,
    PPT_ITEM_TEXT_CISTYLE_ATOM             = 4008,
    PPT_ITEM_TEXT_SPEC_INFO                = 4010,
    PPT_ITEM_EX_OLE_OBJ_STG                = 4113,
    PPT_ITEM_CSTRING                       = 4026,
    PPT_ITEM_HANDOUT                       = 4041,
    PPT_ITEM_HEADERS_FOOTERS               = 4057,
    PPT_ITEM_HEADERS_FOOTERS_ATOM          = 4058,
    PPT_ITEM_SLIDE_LIST_WITH_TEXT          = 4080,
    PPT_ITEM_SLIDE_LIST                    = 4084,
    PPT_ITEM_USER_EDIT_ATOM                = 4085,
    PPT_ITEM_PROG_TAGS                     = 5000,
    PPT_ITEM_PROG_STRING_TAG               = 5001,
    PPT_ITEM_PROG_BINARY_TAG               = 5002,
    PPT_ITEM_PERSIST_PTR_INCREMENTAL_BLOCK = 6002,
    PPT_ITEM_UNKNOWN                       = 0
} ppt_item_type_t;

static unsigned int
getushort(unsigned char *buffer, int offset)
{
    return (unsigned short int)buffer[offset] | ((unsigned short int)buffer[offset+1]<<8);
}

static unsigned long int
getulong(unsigned char *buffer, int offset)
{
    return (unsigned long)buffer[offset] | ((unsigned long)buffer[offset+1]<<8) |
        ((unsigned long)buffer[offset+2]<<16) | ((unsigned long)buffer[offset+3]<<24);
}  

/** 
 * process_item:
 *
 * @param rectype 
 * @param reclen 
 * @param input 
 */
static void
process_item(ppt_item_type_t rectype, long reclen,
             GDataInputStream* input, GMemoryInputStream *mem,
             GError **err)
{
    switch (rectype) {
    case PPT_ITEM_DOCUMENT_END:
        g_debug("End of document");
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_DOCUMENT:
        g_debug("Start of document, reclen=%ld", reclen);
        break;

    case PPT_ITEM_DOCUMENT_ATOM:
        g_debug("DocumentAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_SLIDE:
        g_debug("Slide, reclen=%ld"
                "---------------------------------------", reclen);
        break;

    case PPT_ITEM_SLIDE_ATOM:
        g_debug("SlideAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;
		
    case PPT_ITEM_SLIDE_BASE:
        g_debug("SlideBase, reclen=%ld", reclen);
        break;

    case PPT_ITEM_SLIDE_BASE_ATOM:
        g_debug("SlideBaseAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;
		
    case PPT_ITEM_NOTES:
        g_debug("Notes, reclen=%ld", reclen);
        break;

    case PPT_ITEM_NOTES_ATOM:
        g_debug("NotesAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;
		
    case PPT_ITEM_HEADERS_FOOTERS:
        g_debug("HeadersFooters, reclen=%ld", reclen);
        break;

    case PPT_ITEM_HEADERS_FOOTERS_ATOM:
        g_debug("HeadersFootersAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;
		
    case PPT_ITEM_MAIN_MASTER:
        g_debug("MainMaster, reclen=%ld", reclen);
        break;
		
    case PPT_ITEM_TEXT_BYTES_ATOM: {
        gchar *buf;
        gssize i, size;

        g_debug("TextBytes, reclen=%ld", reclen);
        buf = g_malloc(reclen);
        if (!g_input_stream_read_all((GInputStream *)input, buf, reclen, &size, NULL, err)) {
            break;
        }
        for (i = 0; i < size; i++) {
            if ((unsigned char)buf[i] == '\r') {
                buf[i] = '\n';
            }
        }
        g_memory_input_stream_add_data(mem, buf, size, g_free);
        break;
    }
		
    case PPT_ITEM_TEXT_CHARS_ATOM: 
    case PPT_ITEM_CSTRING: {
        int u;
        gsize size;
        GString *str;

        g_debug("CString, reclen=%ld", reclen);
        size = reclen / 2;
        str = g_string_sized_new(size);
        while (size > 0) {
            u = g_data_input_stream_read_uint16(input, NULL, err);
            if (u == '\r') {
                u = '\n';
            }
            g_string_append_unichar(str, (gunichar)u);
        }
        size = str->len;
        g_memory_input_stream_add_data(mem, g_string_free(str, FALSE), size, g_free);
        break;
    }
		
    case PPT_ITEM_USER_EDIT_ATOM:
        g_debug("UserEditAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_COLOR_SCHEME_ATOM:
        g_debug("ColorSchemeAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_PPDRAWING:
        g_debug("PPDrawing, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_ENVIRONMENT:
        g_debug("Environment, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_SSDOC_INFO_ATOM:
        g_debug("SSDocInfoAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_SSSLIDE_INFO_ATOM:
        g_debug("SSSlideInfoAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_PROG_TAGS:
        g_debug("ProgTags, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_PROG_STRING_TAG:
        g_debug("ProgStringTag, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_PROG_BINARY_TAG:
        g_debug("ProgBinaryTag, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_LIST:
        g_debug("List, reclen=%ld", reclen);
        break;

    case PPT_ITEM_SLIDE_LIST_WITH_TEXT:
        g_debug("SlideListWithText, reclen=%ld"
                "---------------------------------------", reclen);
        break;

    case PPT_ITEM_PERSIST_PTR_INCREMENTAL_BLOCK:
        g_debug("PersistPtrIncrementalBlock, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_EX_OLE_OBJ_STG:
        g_debug("ExOleObjStg, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_PPDRAWING_GROUP:
        g_debug("PpdrawingGroup, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_EX_OBJ_LIST:
        g_debug("ExObjList, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_TX_MASTER_STYLE_ATOM:
        g_debug("TxMasterStyleAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_HANDOUT:
        g_debug("Handout, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_SLIDE_PERSIST_ATOM:
        g_debug("SlidePersistAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_TEXT_HEADER_ATOM:
        g_debug("TextHeaderAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_TEXT_SPEC_INFO:
        g_debug("TextSpecInfo, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    case PPT_ITEM_STYLE_TEXT_PROP_ATOM:
        g_debug("StyleTextPropAtom, reclen=%ld", reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);
        break;

    default:
        g_debug("Default action for rectype=%d reclen=%ld",
                rectype, reclen);
        g_input_stream_skip((GInputStream *)input, reclen, NULL, err);

    }
}

static gboolean
chupa_feed_ppt(ChupaText *chupar, ChupaTextInput *input, GError **error)
{
    const char *filename = chupa_text_input_get_filename(input);
    GInputStream *mem;
    GDataInputStream *inp;

    if (strcmp(filename, "PowerPoint Document")) {
        return TRUE;
    }
    inp = G_DATA_INPUT_STREAM(chupa_text_input_get_stream(input));
    g_data_input_stream_set_byte_order(inp, G_DATA_STREAM_BYTE_ORDER_LITTLE_ENDIAN);
    mem = g_memory_input_stream_new();
    input = chupa_text_input_new_from_stream(NULL, mem, filename);
    chupa_text_feed(chupar, input, error);
    do {
        unsigned short rectype;
        unsigned long reclen;
        unsigned char recbuf[8];
        gssize size = g_input_stream_read((GInputStream *)inp, recbuf, sizeof(recbuf), NULL, error);
        if (size == 0) {
            process_item(PPT_ITEM_DOCUMENT_END, 0,
                         inp, (GMemoryInputStream *)mem, error);
            break;
        }
        if (size < sizeof(recbuf)) {
            break;
        }
        rectype = getushort(recbuf, 2);
        reclen = getulong(recbuf, 4);
        if (reclen < 0) {
            break;
        }
        process_item((ppt_item_type_t)rectype, reclen,
                     inp, (GMemoryInputStream *)mem, error);
    } while (1);
    return TRUE;
}

static void
chupa_ppt_decomposer_init(ChupaPPTDecomposer *dec)
{
}
static void
chupa_ppt_decomposer_class_init(ChupaPPTDecomposerClass *klass)
{
    ChupaArchiveDecomposerClass *dec_class = CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass);
    dec_class->get_infile = gsf_infile_msole_new;
    dec_class->feed_component = chupa_feed_ppt;
}

static GType
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
    GType type = chupa_type_ppt_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_ARCHIVE_DECOMPOSER,
                                           "ChupaPPTDecomposer",
                                           &info, 0);
        chupa_type_ppt_decomposer = type;
        chupa_decomposer_register("application/vnd.ms-powerpoint", type);
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
    return g_object_new_valist(CHUPA_TYPE_PPT_DECOMPOSER,
                               first_property, var_args);
}
