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

#ifndef __CHUPA_DISPATCHER_H__
#define __CHUPA_DISPATCHER_H__

#include <glib-object.h>

#include "chupatext/chupa_decomposer.h"

G_BEGIN_DECLS

#define CHUPA_TYPE_DISPATCHER            (chupa_dispatcher_get_type ())
#define CHUPA_DISPATCHER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_DISPATCHER, ChupaDispatcher))
#define CHUPA_DISPATCHER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_DISPATCHER, ChupaDispatcherClass))
#define CHUPA_IS_DISPATCHER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_DISPATCHER))
#define CHUPA_IS_DISPATCHER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_DISPATCHER))
#define CHUPA_DISPATCHER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_DISPATCHER, ChupaDispatcherClass))

typedef struct _ChupaDispatcher         ChupaDispatcher;
typedef struct _ChupaDispatcherClass    ChupaDispatcherClass;

struct _ChupaDispatcher
{
    GObject object;
};

struct _ChupaDispatcherClass
{
    GObjectClass parent_class;
};

GType        chupa_dispatcher_get_type       (void) G_GNUC_CONST;

ChupaDispatcher *chupa_dispatcher_new        (void);
const gchar *chupa_dispatcher_get_module_dir (ChupaDispatcher *builder);
ChupaDecomposer *chupa_dispatcher_dispatch   (ChupaDispatcher *builder,
                                              const gchar *mime_type);

G_END_DECLS

#endif /* __CHUPA_DISPATCHER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
