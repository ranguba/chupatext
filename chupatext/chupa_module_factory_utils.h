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

#ifndef __CHUPA_MODULE_FACTORY_UTILS_H__
#define __CHUPA_MODULE_FACTORY_UTILS_H__

#include <glib-object.h>

#include <chupatext/chupa_module.h>

G_BEGIN_DECLS

void         chupa_module_factory_init        (void);
void         chupa_module_factory_quit        (void);

const gchar *chupa_module_factory_get_default_module_dir   (void);
void         chupa_module_factory_set_default_module_dir   (const gchar *dir);
const gchar *chupa_module_factory_get_module_dir           (void);

void         chupa_module_factory_load        (const gchar *dir,
                                               const gchar *type);
void         chupa_module_factory_load_all    (const gchar *base_dir);
void         chupa_module_factory_unload      (void);
GList       *chupa_module_factory_get_names   (const gchar *type);
gboolean     chupa_module_factory_exist_module(const gchar *type,
                                               const gchar *name);
ChupaModule *chupa_module_factory_load_module (const gchar *type,
                                               const gchar *name);

G_END_DECLS

#endif /* __CHUPA_MODULE_FACTORY_UTILS_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
