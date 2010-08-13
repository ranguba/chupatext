/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext.h"
#include <glib.h>
#include <tar.h>

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

#define roomof(count, unit) (((count) + (unit) - 1) / (unit))
#define roundup(count, unit) (roomof(count, unit) * (unit))

static int
chupa_tar_foreach(ChupaSwitcher *chupar, GInputStream *input, ChupaCallbackFunc *func, void *arg)
{
    struct tar_header header;
    gssize size;
    guchar name[sizeof(header.name) + sizeof(header.prefix) + 2];

    while ((size = g_input_stream_read(input, &header, sizeof(header), 0, 0)) > 0) {
        guchar *p;
        if (size < sizeof(header)) {
            g_warning("garbage in tar file\n");
            break;
        }
        if (memcmp(header.magic, TMAGIC, TMAGLEN) != 0) {
            g_warning("bad magic\n");
            break;
        }
        size = strtoul(header.size);
        if (header.typeflag[0] == REGTYPE || header.typeflag[0] == AREGTYPE) {
            /* regular file only */
            GInputStream *part;
            ChupaSwitcher *subchupar;
            int ret;
            p = name;
            if (header.prefix[0]) {
                p += strlcpy(p, header.prefix, sizeof(header.prefix));
                *p++ = '/';
            }
            p += g_strlcpy(p, header.name, sizeof(header.name));
            /* TODO: make delimited stream */
            part = g_memory_input_stream_new();
            subchupar = chupa_switcher_new(part);
            ret = chupa_foreach(subchupar, func, arg);
            g_object_unref(part);
            g_free(subchupar);
            if (ret) break;
        }
        g_input_stream_skip(input, roundup((gsize)size, 512), NULL, NULL);
    }
    return 0;
}

static const ChupaExtractor
chupa_tar_extactor = {
    {"application", "x-tar"},
    NULL,
    chupa_tar_foreach,
};

void
chupa_module_tar_init(void)
{
    chupa_regsiter_extactor(&chupa_tar_extactor);
}
