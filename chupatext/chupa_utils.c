/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
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

guint
chupa_utils_flags_from_string (GType        flags_type,
                               const gchar *flags_string)
{
    gchar **names, **name;
    GFlagsClass *flags_class;
    guint flags = 0;

    if (!flags_string)
        return 0;

    names = g_strsplit(flags_string, "|", 0);
    flags_class = g_type_class_ref(flags_type);
    for (name = names; *name; name++) {
        if (g_str_equal(*name, "all")) {
            flags |= flags_class->mask;
            break;
        } else {
            GFlagsValue *value;

            value = g_flags_get_value_by_nick(flags_class, *name);
            if (value)
                flags |= value->value;
        }
    }
    g_type_class_unref(flags_class);
    g_strfreev(names);

    return flags;
}

gint
chupa_utils_enum_from_string (GType        enum_type,
                              const gchar *enum_string)
{
    GEnumClass *enum_class;
    GEnumValue *enum_value;
    gint value = 0;

    if (!enum_string)
        return 0;

    enum_class = g_type_class_ref(enum_type);
    enum_value = g_enum_get_value_by_nick(enum_class, enum_string);
    if (enum_value)
        value = enum_value->value;
    g_type_class_unref(enum_class);

    return value;
}

gboolean
chupa_utils_guess_console_color_usability (void)
{
    const gchar *term;

    term = g_getenv("TERM");
    if (term && (g_str_has_suffix(term, "term") ||
                 g_str_has_suffix(term, "term-color") ||
                 g_str_equal(term, "screen") ||
                 g_str_equal(term, "linux")))
        return TRUE;

    return FALSE;
}

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
chupa_win32_build_decomposer_module_dir_name (const gchar *type)
{
    gchar *module_dir, *decopmoser_module_dir;

    module_dir = chupa_win32_build_module_dir_name("decopmoser");
    decopmoser_module_dir = g_build_filename(module_dir, type, NULL);
    g_free(module_dir);
    return decopmoser_module_dir;
}
#endif


/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
