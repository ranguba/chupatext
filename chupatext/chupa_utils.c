/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <chupatext/config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <math.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <glib.h>
#include <glib/gstdio.h>
#ifdef G_OS_WIN32
#  include <winsock2.h>
#  include <io.h>
#  define close _close
#endif

#include <errno.h>

#include "chupa_utils.h"

#ifdef G_OS_WIN32
static gchar *win32_base_path = NULL;

const gchar *
chupa_win32_base_path (void)
{
    if (win32_base_path)
        return win32_base_path;

    win32_base_path = g_win32_get_package_installation_directory_of_module(NULL);

    return win32_base_path;
}

gchar *
chupa_win32_build_module_dir_name (const gchar *type)
{
    return g_build_filename(chupa_win32_base_path(), "lib", PACKAGE,
                            "module", type, NULL);
}

gchar *
chupa_win32_build_factory_module_dir_name (const gchar *type)
{
    gchar *module_dir, *factory_module_dir;

    module_dir = chupa_win32_build_module_dir_name("factory");
    factory_module_dir = g_build_filename(module_dir, type, NULL);
    g_free(module_dir);
    return factory_module_dir;
}
#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
