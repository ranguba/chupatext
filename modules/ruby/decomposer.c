/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

#include "chupa_ruby.h"

CHUPA_RUBY_DEF_EXCEPTION_METHODS(Decomposer, DECOMPOSER);

VALUE
chupa_ruby_decomposer_init(VALUE mChupa, VALUE eChupaGError)
{
    VALUE cDecomposer;

    cDecomposer = G_DEF_CLASS(CHUPA_TYPE_DECOMPOSER, "Decomposer", mChupa);

    CHUPA_RUBY_DEF_EXCEPTION_CLASS(Decomposer, DECOMPOSER);

    return cDecomposer;
}
