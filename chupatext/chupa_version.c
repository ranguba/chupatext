/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

#include "chupa_version.h"

gint
chupa_version(void)
{
    return CHUPA_VERSION;
}

gint
chupa_version_major(void)
{
    return CHUPA_VERSION_MAJOR;
}

gint
chupa_version_minor(void)
{
    return CHUPA_VERSION_MINOR;
}

gint
chupa_version_micro(void)
{
    return CHUPA_VERSION_MICRO;
}

const gchar *
chupa_version_string(void)
{
    return CHUPA_VERSION_STRING;
}

gint
chupa_n_commits(void)
{
    return CHUPA_N_COMMITS;
}

gint
chupa_release_date(void)
{
    return CHUPA_RELEASE_DATE;
}

const gchar *
chupa_version_description(void)
{
    return CHUPA_VERSION_DESCRIPTION;
}

const gchar chupa_ident[] = "$Ident: " CHUPA_VERSION_DESCRIPTION " $";
