/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef CHUPA_MODULE_IMPL_H
#define CHUPA_MODULE_IMPL_H

#include <glib-object.h>

G_BEGIN_DECLS

#include <chupatext/chupa_module.h>

typedef GList   *(*ChupaModuleInitFunc)(GTypeModule *module);
typedef void     (*ChupaModuleExitFunc)(void);
typedef GObject *(*ChupaModuleCreateFactoryFunc)(const gchar *first_property,
                                                 va_list      va_args);

ChupaModule *
chupa_module_new(const gchar *name,
                 ChupaModuleInitFunc init,
                 ChupaModuleExitFunc exit,
                 ChupaModuleCreateFactoryFunc create_factory);

G_END_DECLS

#endif /* CHUPA_MODULE_IMPL_H */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
