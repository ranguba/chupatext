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

#ifndef __CHUPA_UTILS_H__
#define __CHUPA_UTILS_H__

#include <glib-object.h>

G_BEGIN_DECLS

guint        chupa_utils_flags_from_string   (GType        flags_type,
                                              const gchar *flags_string);
gint         chupa_utils_enum_from_string
                                             (GType        enum_type,
                                              const gchar *enum_string);
gboolean     chupa_utils_guess_console_color_usability
                                             (void);
gboolean     chupa_utils_string_equal        (const gchar *string1,
                                              const gchar *string2);
gchar       *chupa_utils_format_rfc2822      (GTimeVal    *time_value);


#ifdef G_OS_WIN32
const gchar *chupa_win32_base_path           (void);
gchar       *chupa_win32_build_module_dir_name
                                             (const gchar *type);
gchar       *chupa_win32_build_decomposer_module_dir_name
                                             (const gchar *type);
#endif

G_END_DECLS

#endif /* __CHUPA_UTILS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
