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

#include <glib.h>
#include <string.h>

#include "chupa_decomposer.h"
#include "chupa_module.h"

#define CHUPA_DECOMPOSER_GET_PRIVATE(obj)                       \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj,                           \
                                 CHUPA_TYPE_DECOMPOSER_OBJECT,  \
                                 ChupaDecomposerPrivate))

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
typedef struct _ChupaDecomposerPrivate  ChupaDecomposerPrivate;
struct _ChupaDecomposerPrivate
{
};
#endif

G_DEFINE_ABSTRACT_TYPE(ChupaDecomposer, chupa_decomposer, G_TYPE_OBJECT)

static void
chupa_decomposer_init(ChupaDecomposer *decomposer)
{
}

static void
chupa_decomposer_class_init(ChupaDecomposerClass *klass)
{
#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    g_type_class_add_private(gobject_class, sizeof(ChupaDecomposerPrivate));
#endif
}

gboolean
chupa_decomposer_feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
                      ChupaTextInput *input, GError **error)
{
    ChupaDecomposerClass *decomposer_class;

    g_return_val_if_fail(CHUPA_IS_DECOMPOSER(decomposer), FALSE);
    decomposer_class = CHUPA_DECOMPOSER_GET_CLASS(decomposer);
    return decomposer_class->feed(decomposer, feeder, input, error);
}
