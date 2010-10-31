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

#ifndef CHUPA_DECOMPOSER_MODULE_H
#define CHUPA_DECOMPOSER_MODULE_H

#include <glib-object.h>

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_decomposer_factory.h>

G_BEGIN_DECLS

#define CHUPA_DECOMPOSER_INIT           chupa_decomposer_init
#define CHUPA_DECOMPOSER_QUIT           chupa_decomposer_quit
#define CHUPA_DECOMPOSER_CREATE_FACTORY chupa_decomposer_create_factory

GList   *CHUPA_DECOMPOSER_INIT          (GTypeModule *module,
                                         GError     **error);
gboolean CHUPA_DECOMPOSER_QUIT          (void);
GObject *CHUPA_DECOMPOSER_CREATE_FACTORY(const gchar *first_property,
                                         va_list      va_args);

G_END_DECLS

#endif /* CHUPA_DECOMPOSER_MODULE_H */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
