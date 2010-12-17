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

#define SELF(self) (CHUPA_LOGGER(RVAL2GOBJ(self)))
#define LOG_LEVEL2RVAL(level) GFLAGS2RVAL(level, CHUPA_TYPE_LOG_LEVEL_FLAGS)
#define RVAL2LOG_LEVEL(level) RVAL2GFLAGS(level, CHUPA_TYPE_LOG_LEVEL_FLAGS)
#define LOG_ITEM2RVAL(item) GFLAGS2RVAL(item, CHUPA_TYPE_LOG_ITEM_FLAGS)

static VALUE
s_from_string(VALUE self, VALUE string)
{
    return LOG_LEVEL2RVAL(chupa_log_level_flags_from_string(RVAL2CSTR(string)));
}

static VALUE
s_default(VALUE self)
{
    return GOBJ2RVAL(chupa_logger());
}

static VALUE
log_full(VALUE self, VALUE domain, VALUE level, VALUE file, VALUE line,
         VALUE function, VALUE message)
{
    chupa_logger_log(SELF(self),
		      RVAL2CSTR(domain),
		      RVAL2LOG_LEVEL(level),
		      RVAL2CSTR(file),
		      NUM2UINT(line),
		      RVAL2CSTR(function),
		      "%s",
		      RVAL2CSTR(message));
    return Qnil;
}

VALUE
chupa_ruby_logger_init(VALUE mChupa, VALUE eChupaError)
{
    VALUE cChupaLogger;
    VALUE cChupaLogLevelFlags;
    VALUE cChupaLogItemFlags;

    cChupaLogger = G_DEF_CLASS(CHUPA_TYPE_LOGGER, "Logger", mChupa);

    cChupaLogLevelFlags =
	G_DEF_CLASS(CHUPA_TYPE_LOG_LEVEL_FLAGS, "LogLevelFlags", mChupa);
    cChupaLogItemFlags =
	G_DEF_CLASS(CHUPA_TYPE_LOG_ITEM_FLAGS, "LogItemFlags", mChupa);
    G_DEF_CLASS(CHUPA_TYPE_LOG_COLORIZE, "LogColorize", mChupa);

    G_DEF_CONSTANTS(mChupa, CHUPA_TYPE_LOG_LEVEL_FLAGS, "CHUPA_");
    G_DEF_CONSTANTS(mChupa, CHUPA_TYPE_LOG_ITEM_FLAGS, "CHUPA_");
    G_DEF_CONSTANTS(mChupa, CHUPA_TYPE_LOG_COLORIZE, "CHUPA_");

    rb_define_const(cChupaLogLevelFlags,
                    "ALL",
                    LOG_LEVEL2RVAL(CHUPA_LOG_LEVEL_ALL));
    rb_define_const(mChupa,
                    "LOG_LEVEL_ALL",
                    LOG_LEVEL2RVAL(CHUPA_LOG_LEVEL_ALL));
    rb_define_const(cChupaLogItemFlags,
                    "ALL",
                    LOG_ITEM2RVAL(CHUPA_LOG_ITEM_ALL));
    rb_define_const(mChupa,
                    "LOG_ITEM_ALL",
                    LOG_ITEM2RVAL(CHUPA_LOG_ITEM_ALL));

    rb_define_singleton_method(cChupaLogLevelFlags, "from_string",
                               s_from_string, 1);

    rb_define_singleton_method(cChupaLogger, "default", s_default, 0);

    rb_define_method(cChupaLogger, "log_full", log_full, 6);

    G_DEF_SETTERS(cChupaLogger);

    return cChupaLogger;
}
