/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_H
#define CHUPATEXT_H

#include <gio/gio.h>

typedef struct ChupaSwitcher ChupaSwitcher;
typedef struct ChupaModule ChupaModule;
typedef int ChupaCallbackFunc(GInputStream *cabra, gpointer arg);

int chupa_init(void *);
int chupa_cleanup(void);
ChupaSwitcher *chupa_switcher_new(GInputStream *cabra);
int chupa_foreach(ChupaSwitcher *chupar, ChupaCallbackFunc *func, gpointer arg);

typedef struct ChupaExtractor ChupaExtractor;
struct ChupaExtractor {
    struct {
        const char *main, *sub;
    } content_type;
    GInputStream *(*make_converter_input)(GInputStream *);
    int (*foreach)(ChupaSwitcher *, GInputStream *, ChupaCallbackFunc *, gpointer);
};

ChupaExtractor *chupa_extractor_new(ChupaSwitcher *chupar, const char *content_type);

#endif
