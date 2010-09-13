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

const char *
chupa_version_string(void)
{
    return CHUPA_VERSION_STRING;
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

#define STRINGIFY_ARG(x) #x
#define STRINGIFY(x) STRINGIFY_ARG(x)
#define CHUPA_DESCRIPTION "ChupaText " \
    CHUPA_VERSION_STRING "-" STRINGIFY(CHUPA_COMMITS)

const char *
chupa_version_description(void)
{
    return CHUPA_DESCRIPTION;
}

const char chupa_ident[] = "$Ident: " CHUPA_DESCRIPTION
    " $";
