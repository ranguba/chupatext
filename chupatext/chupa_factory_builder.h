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

#ifndef __CHUPA_FACTORY_BUILDER_H__
#define __CHUPA_FACTORY_BUILDER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_FACTORY_BUILDER            (chupa_factory_builder_get_type ())
#define CHUPA_FACTORY_BUILDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_FACTORY_BUILDER, ChupaFactoryBuilder))
#define CHUPA_FACTORY_BUILDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_FACTORY_BUILDER, ChupaFactoryBuilderClass))
#define CHUPA_IS_FACTORY_BUILDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_FACTORY_BUILDER))
#define CHUPA_IS_FACTORY_BUILDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_FACTORY_BUILDER))
#define CHUPA_FACTORY_BUILDER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_FACTORY_BUILDER, ChupaFactoryBuilderClass))

typedef struct _ChupaFactoryBuilder         ChupaFactoryBuilder;
typedef struct _ChupaFactoryBuilderClass    ChupaFactoryBuilderClass;

struct _ChupaFactoryBuilder
{
    GObject object;
};

struct _ChupaFactoryBuilderClass
{
    GObjectClass parent_class;
    void         (*set_option_context) (ChupaFactoryBuilder *builder,
                                        GOptionContext *context);
    GList       *(*build)              (ChupaFactoryBuilder *builder);
    GList       *(*build_all)          (ChupaFactoryBuilder *builder);
    const gchar *(*get_type_name)      (ChupaFactoryBuilder *builder);
};

GType        chupa_factory_builder_get_type       (void) G_GNUC_CONST;
const gchar *chupa_factory_builder_get_type_name  (ChupaFactoryBuilder *builder);
const gchar *chupa_factory_builder_get_module_dir (ChupaFactoryBuilder *builder);
void         chupa_factory_builder_set_option_context
                                                  (ChupaFactoryBuilder *builder,
                                                   GOptionContext *context);
GList       *chupa_factory_builder_build          (ChupaFactoryBuilder *builder);
GList       *chupa_factory_builder_build_all      (ChupaFactoryBuilder *builder);

G_END_DECLS

#endif /* __CHUPA_FACTORY_BUILDER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
