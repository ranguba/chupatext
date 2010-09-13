/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_H
#define CHUPATEXT_H

#include <chupatext/chupa_text.h>

G_BEGIN_DECLS

int chupa_init(void *);
int chupa_cleanup(void);

const int chupa_version(void) G_GNUC_CONST;
const int chupa_commits(void) G_GNUC_CONST;
const int chupa_release_date(void) G_GNUC_CONST;

G_END_DECLS

#endif  /* CHUPATEXT_H */
