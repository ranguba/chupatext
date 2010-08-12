/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_H
#define CHUPATEXT_H

#include <gio/gio.h>

typedef struct ChupaSwitcher ChupaSwitcher;

int chupa_init(void *);
int chupa_cleanup(void);
ChupaSwitcher *chupa_switcher_new(GInputStream *cabra);
int chupa_foreach(int (*func)(GInputStream *cabra, void *arg), void *arg);

/* raw interface */
GConverter *chupa_converter(void);
GInputStream *chupa_begin(GInputStream*);
void chupa_finish(GInputStream*);

typedef int chupa_callback_t(const char *str, size_t len, void *arg),
int chupa_extract(chupa_t *chupar, chupa_callback_t *func, void *arg);
int chupa_extract_start(chupa_t *chupar, chupa_callback_t *func, void *arg);
int chupa_extract_wait(chupa_t *chupar);

#endif
