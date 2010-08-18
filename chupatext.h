/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_H
#define CHUPATEXT_H

int chupa_init(void *);
int chupa_cleanup(void);

const int chupa_version(void);
const int chupa_commits(void);
const int chupa_release_date(void);

#endif  /* CHUPATEXT_H */
