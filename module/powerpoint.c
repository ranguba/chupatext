/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/archive_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_gsf_input_stream.h>
#include <glib.h>
#include <gsf/gsf-output-memory.h>
#include <gsf/gsf-infile-msole.h>
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

#if 0
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
#endif

#define WORK_SIZE 128

struct PPT {
    unsigned char working_buffer[WORK_SIZE];
    int buf_idx;
    int output_this_container;
    int past_first_slide;
    int last_container;
    GsfOutput *output;
};

static void
put_byte(struct PPT *ppt, unsigned char c)
{
    gsf_output_write(ppt->output, 1, &c);
}

static void
put_utf8(struct PPT *ppt, unsigned short c)
{
    put_byte(ppt, 0x0080 | ((short)c & 0x003F));
}

static void
print_utf8(struct PPT *ppt, unsigned short c)
{
    if (c == 0)
        return;
		
    if (c < 0x80)
        put_byte(ppt, c);
    else if (c < 0x800) {
        put_byte(ppt, 0xC0 | (c >>  6));
        put_utf8(ppt, c);
    }
    else
    {
        put_byte(ppt, 0xE0 | (c >> 12));
        put_utf8(ppt, c >>  6);
        put_utf8(ppt, c);
    }
}

static void
print_unicode(struct PPT *ppt, unsigned char *ucs, int len)
{
    int i;
    for (i = 0; i < len; i += 2) {
        print_utf8(ppt, ucs[i] | (ucs[i+1] << 8));
    }
}

static void
container_processor(struct PPT *ppt, int type)
{
    if (type == 0x03EE) {
        if (ppt->past_first_slide) {
            put_byte(ppt, '\f');
        }
        else {
            ppt->past_first_slide = 1;
        }
    }
    switch (type) {
    case 0x000D:
        if (ppt->last_container == 0x11) /* suppress notes info */
            ppt->output_this_container = 0;
        else
            ppt->output_this_container = 1;
        break;
    case 0x0FF0:
        ppt->output_this_container = 1;
        break;
    default:
        /* printf("Cont:%x|\n", type);	*/
        ppt->output_this_container = 0;
        break;
    }
    ppt->last_container = type;
}

static void
atom_processor(struct PPT *ppt, int type, int count, int buf_last, unsigned char data)
{
    if ((ppt->buf_idx >= WORK_SIZE) || (ppt->output_this_container == 0))
        return;

    if (count == 0) {
        memset(ppt->working_buffer, 0, WORK_SIZE);
        ppt->buf_idx = 0;
    }
	
    switch (type) {
    case 0x0FA0:	/* Text String in unicode */
        ppt->working_buffer[ppt->buf_idx++] = data;
        if (count == buf_last) {
            /* printf("Atom:%x|\n", type);	*/
            /* working_buffer[buf_idx++] = 0;       */
            /* printf("%s<BR>\n", working_buffer); */
            print_unicode(ppt, ppt->working_buffer, ppt->buf_idx);
            put_byte(ppt, '\n');
        }
        break;
    case 0x0FA8:	/* Text String in ASCII */
        if (data == '\r') {
            data = '\n';
        }
        ppt->working_buffer[ppt->buf_idx++] = data;
        if (count == buf_last) {
            /* ppt->working_buffer[ppt->buf_idx++] = 0;	*/
            /* printf("Atom:%x|\n", type);	*/
            gsf_output_write(ppt->output, ppt->buf_idx, ppt->working_buffer);
            put_byte(ppt, '\n');
        }
        break;
    case 0x0FBA:	/* CString - unicode... */
        if (data == '\r') {
            data = '\n';
        }
        ppt->working_buffer[ppt->buf_idx++] = data;
        if (count == buf_last) {
            /* working_buffer[buf_idx++] = 0;	*/
            /* printf("%s<BR>\n", working_buffer); */
            /* printf("Atom:%x|\n", type);	*/
            print_unicode(ppt, ppt->working_buffer, ppt->buf_idx);
            put_byte(ppt, '\n');
        }
        break;
    default:
        break;
    }
}

static gboolean
dump_file(struct PPT *ppt, GsfInput *inp)
{
    unsigned long version=0, instance=0, type=0, length=0, target=0, count=0;
    unsigned char buf[16];

    /* Output body */
    while (gsf_input_read(inp, 1, buf)) {
        switch (count) {
        case 0:
            instance = buf[0];
            type = 0;
            length = 0;
            target = 80;	/* ficticious number */
            break;
        case 1:
            instance |= (buf[0]<<8);
            version = instance &0x000F;
            instance = (instance>>4);
            break;
        case 2:
            type = (unsigned)buf[0];
            break;
        case 3:
            type |= (buf[0]<<8)&0x00000FFFL;
            break;
        case 4:
            length = (unsigned)buf[0];
            break;
        case 5:
            length |= (buf[0]<<8);
            break;
        case 6:
            length |= (buf[0]<<16);
            break;
        case 7:
            length |= (buf[0]<<24);
            target = length;
            if (version == 0x0F) {
            	/* Do container level Processing */
                container_processor(ppt, type);
                count = -1;
            }
            break;
        }
        if (count > 7) {
            /* Here is where we want to process the data
               based on the Atom type... */
            atom_processor(ppt, type, count-8, target-1, buf[0]);
        }
        if (count == (target+7))
            count = 0;
        else
            count++;
    }

    if (ppt->past_first_slide) {
        put_byte(ppt, '\f');
    }
    return TRUE;
}

static gboolean
chupa_feed_ppt(ChupaText *chupar, ChupaTextInput *input, GError **error)
{
    const char *filename = chupa_text_input_get_filename(input);
    GsfOutput *mem;
    GsfInput *gin = chupa_text_input_get_base_input(input);
    GInputStream *inp;
    struct PPT ppt;

    if (strcmp(filename, "PowerPoint Document")) {
        return TRUE;
    }
    mem = gsf_output_memory_new();
    inp = chupa_gsf_input_stream_new(GSF_OUTPUT_MEMORY(mem));
    input = chupa_text_input_new_from_stream(NULL, inp, filename);
    chupa_text_feed(chupar, input, error);
    memset(&ppt, 0, sizeof(ppt));
    gsf_input_seek(gin, 0, G_SEEK_SET);
    ppt.output = mem;
    while (dump_file(&ppt, gin)) {
    }
    return TRUE;
}

static void
chupa_ppt_decomposer_class_init(ChupaPPTDecomposerClass *klass)
{
    ChupaArchiveDecomposerClass *dec_class = CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass);
    dec_class->get_infile = gsf_infile_msole_new;
    dec_class->feed_component = chupa_feed_ppt;
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
                                    CHUPA_TYPE_ARCHIVE_DECOMPOSER,
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