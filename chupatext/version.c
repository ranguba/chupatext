/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "version.h"

const int
chupa_version(void)
{
    return CHUPA_VERSION_CODE;
}

const int
chupa_commits(void)
{
    return CHUPA_COMMITS;
}

const int
chupa_release_date(void)
{
    return CHUPA_RELEASE_DATE;
}

#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)
const char chupa_ident[] = "$Ident: ChupaText " CHUPA_VERSION_STRING "-" STRINGIZE2(CHUPA_COMMITS) " $";
