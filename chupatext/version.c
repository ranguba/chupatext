/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include "version.h"

int
chupa_version(void)
{
    return CHUPA_VERSION_CODE;
}

const char *
chupa_version_string(void)
{
    return CHUPA_VERSION_STRING;
}

int
chupa_commits(void)
{
    return CHUPA_COMMITS;
}

int
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
