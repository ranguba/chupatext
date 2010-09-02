/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_restrict_input_stream.h>
#include <glib.h>
#include <tar.h>

#define TAR_PAGE_SIZE 512

struct tar_header {
    guchar name[100];		/* NUL-terminated if NUL fits */
    guchar mode[8];
    guchar uid[8];
    guchar gid[8];
    guchar size[12];
    guchar mtime[12];
    guchar chksum[8];
    guchar typeflag[1];
    guchar linkname[100];	/* NUL-terminated if NUL fits */
    guchar magic[6];		/* must be TMAGIC (NUL term.) */
    guchar version[2];		/* must be TVERSION */
    guchar uname[32];		/* NUL-terminated */
    guchar gname[32];		/* NUL-terminated */
    guchar devmajor[8];
    guchar devminor[8];
    guchar prefix[155];		/* NUL-terminated if NUL fits */
};

union tar_buffer {
    struct tar_header bytes;
    unsigned int uints[TAR_PAGE_SIZE / sizeof(unsigned int)];
};

G_STATIC_ASSERT(sizeof(union tar_buffer) == TAR_PAGE_SIZE);

#define roomof(count, unit) (((count) + (unit) - 1) / (unit))
#define roundup(count, unit) (roomof(count, unit) * (unit))

#define CHUPA_TYPE_TAR_DECOMPOSER            chupa_type_tar_decomposer
#define CHUPA_TAR_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTARDecomposer)
#define CHUPA_TAR_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTARDecomposerClass)
#define CHUPA_IS_TAR_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TAR_DECOMPOSER)
#define CHUPA_IS_TAR_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TAR_DECOMPOSER)
#define CHUPA_TAR_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TAR_DECOMPOSER, ChupaTARDecomposerClass)

typedef struct _ChupaTARDecomposer ChupaTARDecomposer;
typedef struct _ChupaTARDecomposerClass ChupaTARDecomposerClass;

struct _ChupaTARDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaTARDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_tar_decomposer = 0;

static gboolean
is_null_page(const unsigned int *up, gsize size)
{
    gsize i = size / sizeof(*up);

    while (--i > 0) {
        if (*up++) {
            return FALSE;
        }
    }
    i = size % sizeof(*up);
    if (i > 0) {
        const unsigned char *cp = (const unsigned char *)up;
        while (--i > 0) {
            if (*cp++) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

static void
chupa_tar_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInputStream *stream)
{
    union tar_buffer header;
    gssize size;
    guchar name[sizeof(header.bytes.name) + sizeof(header.bytes.prefix) + 2];
    GInputStream *input = G_INPUT_STREAM(stream);

    while ((size = g_input_stream_read(input, &header, sizeof(header), 0, 0)) > 0 &&
           !is_null_page(header.uints, size)) {
        if (size < sizeof(header)) {
            g_warning("garbage in tar file\n");
            break;
        }
        if (memcmp(header.bytes.magic, TMAGIC, TMAGLEN) != 0) {
            g_warning("bad magic\n");
            break;
        }
        size = strtoul(header.bytes.size);
        if (header.bytes.typeflag[0] == REGTYPE || header.bytes.typeflag[0] == AREGTYPE) {
            /* regular file only */
            GInputStream *part;
            guchar *p = name;
            if (header.bytes.prefix[0]) {
                p += strlcpy(p, header.bytes.prefix, sizeof(header.bytes.prefix));
                *p++ = '/';
            }
            p += g_strlcpy(p, header.bytes.name, sizeof(header.bytes.name));
            part = chupa_restrict_input_stream_new(input, size);
            chupa_text_feed(chupar, part);
            chupa_restrict_input_stream_skip_to_end(part);
            g_object_unref(part);
            g_input_stream_skip(input, (gsize)size % TAR_PAGE_SIZE, NULL, NULL);
        }
        else {
            g_input_stream_skip(input, roundup((gsize)size, TAR_PAGE_SIZE), NULL, NULL);
        }
    }
}

static void
chupa_tar_decomposer_class_init(ChupaTARDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_tar_decomposer_feed;
}

static GType
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaTARDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_tar_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaTARDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };
    GType type = chupa_type_tar_decomposer;

    if (!type) {
        type = g_type_module_register_type(type_module,
                                           CHUPA_TYPE_DECOMPOSER,
                                           "ChupaTARDecomposer",
                                           &info, 0);
        chupa_type_tar_decomposer = type;
        chupa_decomposer_register("application/x-tar", type);
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
    return g_object_new_valist(CHUPA_TYPE_TAR_DECOMPOSER,
                               first_property, var_args);
}
