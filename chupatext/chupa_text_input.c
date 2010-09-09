/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_text_input.h"
#include "chupatext/chupa_text_input_stream.h"
#include <gio/gio.h>
#include <gsf/gsf-input-gio.h>

#define INPUT_STREAM_FILE_TYPE input_stream_file_get_type()
#define INPUT_STREAM_FILE(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, INPUT_STREAM_FILE_TYPE, InputStreamFile)
#define INPUT_STREAM_FILE_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, INPUT_STREAM_FILE_TYPE, InputStreamFileClass)
#define IS_INPUT_STREAM_FILE(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, INPUT_STREAM_FILE_TYPE)
#define IS_INPUT_STREAM_FILE_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, INPUT_STREAM_FILE_TYPE)
#define INPUT_STREAM_FILE_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, INPUT_STREAM_FILE_TYPE, InputStreamFileClass)

typedef struct {
    GObject parent_instance;

    char *path;
    GInputStream *stream;
} InputStreamFile;

typedef struct {
    GObjectClass parent_class;
} InputStreamFileClass;

static void input_stream_file_file_iface_init(GFileIface *iface);

G_DEFINE_TYPE_WITH_CODE(InputStreamFile, input_stream_file, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(G_TYPE_FILE,
                                              input_stream_file_file_iface_init))

static void
input_stream_file_finalize(GObject *object)
{
    InputStreamFile *isfile = (InputStreamFile *)object;
    g_object_unref(isfile->stream);
}

static GFileInputStream *
input_stream_file_read(GFile *file, GCancellable *cancellable, GError **error)
{
    InputStreamFile *isfile = (InputStreamFile *)file;
    return g_object_ref(isfile->stream);
}

static GFileInfo *
input_stream_file_query_info(GFile *file, const char *attributes,
                             GFileQueryInfoFlags flags,
                             GCancellable *cancellable, GError **error)
{
    InputStreamFile *isfile = (InputStreamFile *)file;
    GFileInfo *info = NULL;
    GSeekable *sk;
    goffset size, curpos;
    if (strcmp(attributes, G_FILE_ATTRIBUTE_STANDARD_NAME) == 0) {
        if (isfile->path) {
            info = g_file_info_new();
            g_file_info_set_attribute_byte_string(info, attributes, isfile->path);
        }
    }
    else if (strcmp(attributes, G_FILE_ATTRIBUTE_STANDARD_SIZE) == 0 &&
             G_IS_SEEKABLE(isfile->stream) &&
             g_seekable_can_seek(sk = G_SEEKABLE(isfile->stream)) &&
             (curpos = g_seekable_tell(sk),
              g_seekable_seek(sk, (goffset)0, G_SEEK_END, NULL, NULL))) {
        size = g_seekable_tell(sk);
        g_seekable_seek(sk, curpos, G_SEEK_SET, NULL, NULL);
        info = g_file_info_new();
        g_file_info_set_attribute_uint64(info, attributes, size);
    }
    return info;
}

static GFile *
input_stream_file_dup(GFile *file)
{
    return g_object_ref(file);
}

static void
input_stream_file_init(InputStreamFile *isfile)
{
    isfile->path = NULL;
    isfile->stream = NULL;
}

static void
input_stream_file_class_init(InputStreamFileClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = input_stream_file_finalize;
}

static void
input_stream_file_file_iface_init(GFileIface *iface)
{
    iface->dup = input_stream_file_dup;
    iface->query_info = input_stream_file_query_info;
    iface->read_fn = input_stream_file_read;
}

GFile *
input_stream_file_new(const char *path, GInputStream *stream)
{
    InputStreamFile *isfile = g_object_new(INPUT_STREAM_FILE_TYPE, NULL);
    isfile->path = g_strdup(path);
    g_object_ref(isfile->stream = stream);
    return G_FILE(isfile);
}


/* ChupaTextInput */
G_DEFINE_TYPE(ChupaTextInput, chupa_text_input, G_TYPE_OBJECT)

#define CHUPA_TEXT_INPUT_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), \
                                 CHUPA_TYPE_TEXT_INPUT, \
                                 ChupaTextInputPrivate))

typedef struct _ChupaTextInputPrivate ChupaTextInputPrivate;

struct _ChupaTextInputPrivate
{
    GsfInput *input;
    ChupaMetadata *metadata;
    GDataInputStream *stream;
};

enum {
    PROP_0,
    PROP_INPUT,
    PROP_METADATA,
    PROP_DUMMY
};

enum {
    peek_buffer_size = 1024
};

static const char *
guess_mime_type(const char *name, GBufferedInputStream *buffered, gboolean *uncertain)
{
    const char *content_type;
    gsize len;
    const char *buf;

    g_buffered_input_stream_fill(buffered, peek_buffer_size, NULL, NULL);
    buf = g_buffered_input_stream_peek_buffer(buffered, &len);
    content_type = g_content_type_guess(name, buf, len, uncertain);
    return g_content_type_get_mime_type(content_type);
}

static void
chupa_text_input_init(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    priv->stream = NULL;
    priv->metadata = NULL;
}

static void
constructed(GObject *object)
{
    ChupaTextInput *input = CHUPA_TEXT_INPUT(object);
    ChupaTextInputPrivate *priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    const gchar *mime_type;
    GInputStream *stream;

    if (!priv->metadata) {
        priv->metadata = chupa_metadata_new();
    }
    stream = G_INPUT_STREAM(chupa_text_input_stream_new(input));
    if (G_IS_DATA_INPUT_STREAM(stream)) {
        priv->stream = G_DATA_INPUT_STREAM(stream);
    }
    else {
        priv->stream = g_data_input_stream_new(stream);
    }

    mime_type = guess_mime_type(gsf_input_name(priv->input),
                                G_BUFFERED_INPUT_STREAM(priv->stream),
                                NULL);
    chupa_metadata_replace_value(priv->metadata, "mime-type", mime_type);
}

static void
dispose(GObject *object)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(object);
    if (priv->metadata) {
        g_object_unref(priv->metadata);
        priv->metadata = NULL;
    }

    G_OBJECT_CLASS(chupa_text_input_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaTextInputPrivate *priv;
    GObject *obj;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        obj = g_value_dup_object(value);
        priv->input = GSF_INPUT(obj);
        break;
    case PROP_METADATA:
        obj = g_value_dup_object(value);
        priv->metadata = CHUPA_METADATA(obj);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property(GObject *object,
             guint prop_id,
             GValue *value,
             GParamSpec *pspec)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_INPUT:
        g_value_set_object(value, priv->stream);
        break;
    case PROP_METADATA:
        g_value_set_object(value, priv->metadata);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
chupa_text_input_class_init(ChupaTextInputClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *spec;

    gobject_class->constructed  = constructed;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_object("input",
                               "Input",
                               "Input",
                               GSF_INPUT_TYPE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_INPUT, spec);

    spec = g_param_spec_object("metadata",
                               "Metadata",
                               "Metadata of the input",
                               CHUPA_TYPE_METADATA,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(gobject_class, PROP_METADATA, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaTextInputPrivate));
}

ChupaTextInput *
chupa_text_input_new(ChupaMetadata *metadata, GsfInput *input)
{
    return g_object_new(CHUPA_TYPE_TEXT_INPUT,
                        "input", input,
                        "metadata", metadata,
                        NULL);
}

ChupaTextInput *
chupa_text_input_new_from_stream(ChupaMetadata *metadata, GInputStream *stream, const char *path)
{
    ChupaTextInput *text;
    GFile *file = input_stream_file_new(path, stream);
    GsfInput *input = gsf_input_gio_new(file, NULL);
    g_object_unref(file);
    text = chupa_text_input_new(metadata, input);
    g_object_unref(input);
    return text;
}

ChupaTextInput *
chupa_text_input_new_from_file(ChupaMetadata *metadata, GFile *file)
{
    ChupaTextInput *text;
    GsfInput *input = gsf_input_gio_new(file, NULL);
    text = chupa_text_input_new(metadata, input);
    g_object_unref(input);
    return text;
}

GsfInput *
chupa_text_input_get_base_input(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    return priv->input;
}

GInputStream *
chupa_text_input_get_stream(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    return G_INPUT_STREAM(priv->stream);
}

ChupaMetadata *
chupa_text_input_get_metadata(ChupaTextInput *input)
{
    ChupaTextInputPrivate *priv;

    priv = CHUPA_TEXT_INPUT_GET_PRIVATE(input);
    return priv->metadata;
}

const gchar *
chupa_text_input_get_mime_type(ChupaTextInput *input)
{
    ChupaMetadata *meta = chupa_text_input_get_metadata(input);
    return chupa_metadata_get_first_value(meta, "mime-type");
}
