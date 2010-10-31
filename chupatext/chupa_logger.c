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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "chupa_logger.h"
#include "chupa_utils.h"
#include "chupa_enum_types.h"
#include "chupa_marshalers.h"

#define DEFAULT_KEY "default"
#define DEFAULT_LEVEL                                   \
    (CHUPA_LOG_LEVEL_CRITICAL | CHUPA_LOG_LEVEL_ERROR)

#define CHUPA_LOGGER_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                \
                                 CHUPA_TYPE_LOGGER,    \
                                 ChupaLoggerPrivate))

typedef struct _ChupaLoggerPrivate	ChupaLoggerPrivate;
struct _ChupaLoggerPrivate
{
    ChupaLogLevelFlags target_level;
    ChupaLogItemFlags target_item;
    GHashTable *interesting_levels;
    ChupaLogLevelFlags interesting_level;
};

enum
{
    PROP_0,
    PROP_TARGET_LEVEL,
    PROP_TARGET_ITEM,
    PROP_INTERESTING_LEVEL
};

enum
{
    LOG,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

static ChupaLogger *singleton_chupa_logger = NULL;

G_DEFINE_TYPE(ChupaLogger, chupa_logger, G_TYPE_OBJECT);

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
chupa_logger_class_init (ChupaLoggerClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_flags("target-level",
                              "Target log level",
                              "The target log level",
                              CHUPA_TYPE_LOG_LEVEL_FLAGS,
                              CHUPA_LOG_LEVEL_DEFAULT,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TARGET_LEVEL, spec);

    spec = g_param_spec_flags("target-item",
                              "Target log item",
                              "The target log item",
                              CHUPA_TYPE_LOG_ITEM_FLAGS,
                              CHUPA_LOG_ITEM_DEFAULT,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TARGET_ITEM, spec);

    spec = g_param_spec_flags("interesting-level",
                              "Interesting log level",
                              "The interesting log level",
                              CHUPA_TYPE_LOG_LEVEL_FLAGS,
                              DEFAULT_LEVEL,
                              G_PARAM_READABLE);
    g_object_class_install_property(gobject_class, PROP_INTERESTING_LEVEL, spec);

    signals[LOG] =
        g_signal_new("log",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(ChupaLoggerClass, log),
                     NULL, NULL,
                     _chupa_marshal_VOID__STRING_FLAGS_STRING_UINT_STRING_POINTER_STRING,
                     G_TYPE_NONE, 7,
                     G_TYPE_STRING, CHUPA_TYPE_LOG_LEVEL_FLAGS, G_TYPE_STRING,
                     G_TYPE_UINT, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_STRING);

    g_type_class_add_private(gobject_class, sizeof(ChupaLoggerPrivate));
}

static void
chupa_logger_init (ChupaLogger *logger)
{
    ChupaLoggerPrivate *priv;

    priv = CHUPA_LOGGER_GET_PRIVATE(logger);
    priv->target_level = CHUPA_LOG_LEVEL_DEFAULT;
    priv->interesting_levels = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                     g_free, NULL);
    priv->interesting_level = DEFAULT_LEVEL;
    g_hash_table_insert(priv->interesting_levels,
                        g_strdup(DEFAULT_KEY),
                        GUINT_TO_POINTER(priv->interesting_level));
}

static void
dispose (GObject *object)
{
    ChupaLoggerPrivate *priv;

    priv = CHUPA_LOGGER_GET_PRIVATE(object);

    if (priv->interesting_levels) {
        g_hash_table_unref(priv->interesting_levels);
        priv->interesting_levels = NULL;
    }

    G_OBJECT_CLASS(chupa_logger_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    ChupaLoggerPrivate *priv;

    priv = CHUPA_LOGGER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_TARGET_LEVEL:
        priv->target_level = g_value_get_flags(value);
        break;
    case PROP_TARGET_ITEM:
        priv->target_item = g_value_get_flags(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    ChupaLoggerPrivate *priv;

    priv = CHUPA_LOGGER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_TARGET_LEVEL:
        g_value_set_flags(value, priv->target_level);
        break;
    case PROP_TARGET_ITEM:
        g_value_set_flags(value, priv->target_item);
        break;
    case PROP_INTERESTING_LEVEL:
        g_value_set_flags(value, priv->interesting_level);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

ChupaLogLevelFlags
chupa_log_level_flags_from_string (const gchar *level_name)
{
    if (chupa_utils_string_equal(level_name, "all")) {
        return CHUPA_LOG_LEVEL_ALL;
    } else {
        return chupa_utils_flags_from_string(CHUPA_TYPE_LOG_LEVEL_FLAGS,
                                             level_name);
    }
}

ChupaLogItemFlags
chupa_log_item_flags_from_string (const gchar *item_name)
{
    return chupa_utils_flags_from_string(CHUPA_TYPE_LOG_ITEM_FLAGS, item_name);
}

#define BLACK_COLOR "\033[01;30m"
#define BLACK_BACK_COLOR "\033[40m"
#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define GREEN_BACK_COLOR "\033[01;42m"
#define YELLOW_COLOR "\033[01;33m"
#define YELLOW_BACK_COLOR "\033[01;43m"
#define BLUE_COLOR "\033[01;34m"
#define BLUE_BACK_COLOR "\033[01;44m"
#define MAGENTA_COLOR "\033[01;35m"
#define MAGENTA_BACK_COLOR "\033[01;45m"
#define CYAN_COLOR "\033[01;36m"
#define CYAN_BACK_COLOR "\033[01;46m"
#define WHITE_COLOR "\033[01;37m"
#define WHITE_BACK_COLOR "\033[01;47m"
#define NORMAL_COLOR "\033[00m"

static void
log_message_colorize_console (GString *log,
                              ChupaLogLevelFlags level,
                              const gchar *message)
{
    const gchar *color = NULL;

    switch (level) {
    case CHUPA_LOG_LEVEL_ERROR:
        color = WHITE_COLOR RED_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_CRITICAL:
        color = YELLOW_COLOR RED_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_WARNING:
        color = WHITE_COLOR YELLOW_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_MESSAGE:
        color = WHITE_COLOR GREEN_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_INFO:
        color = WHITE_COLOR CYAN_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_DEBUG:
        color = WHITE_COLOR BLUE_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_STATISTICS:
        color = BLUE_COLOR WHITE_BACK_COLOR;
        break;
    case CHUPA_LOG_LEVEL_PROFILE:
        color = GREEN_COLOR BLACK_BACK_COLOR;
        break;
    default:
        color = NULL;
        break;
    }

    if (color)
        g_string_append_printf(log, "%s%s%s", color, message, NORMAL_COLOR);
    else
        g_string_append(log, message);
}

static void
log_message (GString *log, ChupaLogLevelFlags level, const gchar *message)
{
    const gchar *colorize_type;
    ChupaLogColorize colorize = CHUPA_LOG_COLORIZE_DEFAULT;

    colorize_type = g_getenv("CHUPA_LOG_COLORIZE");
    if (colorize_type)
        colorize = chupa_utils_enum_from_string(CHUPA_TYPE_LOG_COLORIZE,
                                                colorize_type);

    if (colorize == CHUPA_LOG_COLORIZE_DEFAULT) {

        if (isatty(STDOUT_FILENO) &&
            chupa_utils_guess_console_color_usability()) {
            colorize = CHUPA_LOG_COLORIZE_CONSOLE;
        } else {
            colorize = CHUPA_LOG_COLORIZE_NONE;
        }
    }

    switch (colorize) {
    case CHUPA_LOG_COLORIZE_CONSOLE:
        log_message_colorize_console(log, level, message);
        break;
    default:
        g_string_append(log, message);
        break;
    }
}

void
chupa_logger_default_log_handler (ChupaLogger *logger, const gchar *domain,
                                  ChupaLogLevelFlags level,
                                  const gchar *file, guint line,
                                  const gchar *function,
                                  GTimeVal *time_value, const gchar *message,
                                  gpointer user_data)
{
    ChupaLoggerPrivate *priv;
    ChupaLogLevelFlags target_level;
    ChupaLogItemFlags target_item;
    GString *log;

    priv = CHUPA_LOGGER_GET_PRIVATE(logger);
    target_level = chupa_logger_get_resolved_target_level(logger);
    if (!(level & target_level))
        return;

    log = g_string_new(NULL);

    target_item = priv->target_item;
    if (target_item == CHUPA_LOG_ITEM_DEFAULT)
        target_item = CHUPA_LOG_ITEM_TIME;

    if (target_item & CHUPA_LOG_ITEM_LEVEL) {
        GFlagsClass *flags_class;

        flags_class = g_type_class_ref(CHUPA_TYPE_LOG_LEVEL_FLAGS);
        if (flags_class) {
            if (level & flags_class->mask) {
                guint i;
                for (i = 0; i < flags_class->n_values; i++) {
                    GFlagsValue *value = flags_class->values + i;
                    if (level & value->value)
                        g_string_append_printf(log, "[%s]", value->value_nick);
                }
            }
            g_type_class_unref(flags_class);
        }
    }

    if (domain && (target_item & CHUPA_LOG_ITEM_DOMAIN))
        g_string_append_printf(log, "[%s]", domain);

    if (target_item & CHUPA_LOG_ITEM_TIME) {
        gchar *time_string;

        time_string = g_time_val_to_iso8601(time_value);
        g_string_append_printf(log, "[%s]", time_string);
        g_free(time_string);
    }

    if (file && (target_item & CHUPA_LOG_ITEM_LOCATION)) {
        if (log->len > 0)
            g_string_append(log, " ");
        g_string_append_printf(log, "%s:%d: ", file, line);
        if (function)
            g_string_append_printf(log, "%s(): ", function);
    } else {
        if (log->len > 0)
            g_string_append(log, ": ");
    }

    log_message(log, level, message);
    g_string_append(log, "\n");
    g_print("%s", log->str);
    g_string_free(log, TRUE);
}

ChupaLogger *
chupa_logger (void)
{
    if (!singleton_chupa_logger) {
        singleton_chupa_logger = chupa_logger_new();
        chupa_logger_set_target_level_by_string(singleton_chupa_logger,
                                                g_getenv("CHUPA_LOG_LEVEL"));
        chupa_logger_set_target_item_by_string(singleton_chupa_logger,
                                               g_getenv("CHUPA_LOG_ITEM"));
        chupa_logger_connect_default_handler(singleton_chupa_logger);
    }

    return singleton_chupa_logger;
}

ChupaLogger *
chupa_logger_new (void)
{
    return g_object_new(CHUPA_TYPE_LOGGER,
                        NULL);
}

void
chupa_logger_log (ChupaLogger *logger,
                  const gchar *domain, ChupaLogLevelFlags level,
                  const gchar *file, guint line, const gchar *function,
                  const gchar *format, ...)
{
    va_list args;

    va_start(args, format);
    chupa_logger_log_va_list(logger, domain, level, file, line, function,
                             format, args);
    va_end(args);
}

void
chupa_logger_log_va_list (ChupaLogger *logger,
                          const gchar *domain, ChupaLogLevelFlags level,
                          const gchar *file, guint line, const gchar *function,
                          const gchar *format, va_list args)
{
    GTimeVal time_value;
    gchar *message;

    g_get_current_time(&time_value);

    message = g_strdup_vprintf(format, args);
    g_signal_emit(logger, signals[LOG], 0,
                  domain, level, file, line, function, &time_value, message);
    g_free(message);
}

void
chupa_logger_log_g_error (ChupaLogger *logger,
                          const gchar *domain,
                          const gchar *file, guint line, const gchar *function,
                          GError *error, const gchar *format, ...)
{
    va_list args;
    gchar *message;

    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    va_end(args);

    chupa_logger_log(logger, domain, CHUPA_LOG_LEVEL_ERROR, file, line, function,
                     "%s: <%s> (%s:%d)",
                     message,
                     error->message,
                     g_quark_to_string(error->domain),
                     error->code);
    g_free(message);
}

ChupaLogLevelFlags
chupa_logger_get_target_level (ChupaLogger *logger)
{
    return CHUPA_LOGGER_GET_PRIVATE(logger)->target_level;
}

ChupaLogLevelFlags
chupa_logger_get_resolved_target_level (ChupaLogger *logger)
{
    ChupaLogLevelFlags target_level;

    target_level = CHUPA_LOGGER_GET_PRIVATE(logger)->target_level;
    if (target_level == CHUPA_LOG_LEVEL_DEFAULT)
        target_level = DEFAULT_LEVEL;
    return target_level;
}

void
chupa_logger_set_target_level (ChupaLogger *logger,
                                ChupaLogLevelFlags level)
{
    ChupaLogLevelFlags interesting_level = level;

    CHUPA_LOGGER_GET_PRIVATE(logger)->target_level = level;
    if (interesting_level == CHUPA_LOG_LEVEL_DEFAULT)
        interesting_level = DEFAULT_LEVEL;
    chupa_logger_set_interesting_level(logger, DEFAULT_KEY, interesting_level);
}

void
chupa_logger_set_target_level_by_string (ChupaLogger *logger,
                                         const gchar *level_name)
{
    ChupaLogLevelFlags level;

    if (level_name) {
        level = chupa_log_level_flags_from_string(level_name);
    } else {
        level = CHUPA_LOG_LEVEL_DEFAULT;
    }
    chupa_logger_set_target_level(logger, level);
}

static void
update_interesting_level (gpointer key, gpointer value, gpointer user_data)
{
    ChupaLogLevelFlags *level = user_data;

    *level |= GPOINTER_TO_UINT(value);
}

void
chupa_logger_set_interesting_level (ChupaLogger *logger,
                                     const gchar *name,
                                     ChupaLogLevelFlags level)
{
    ChupaLoggerPrivate *priv;

    priv = CHUPA_LOGGER_GET_PRIVATE(logger);

    g_hash_table_insert(priv->interesting_levels,
                        g_strdup(name),
                        GUINT_TO_POINTER(level));
    priv->interesting_level = 0;
    g_hash_table_foreach(priv->interesting_levels,
                         update_interesting_level,
                         &(priv->interesting_level));
}

ChupaLogLevelFlags
chupa_logger_get_interesting_level (ChupaLogger *logger)
{
    return CHUPA_LOGGER_GET_PRIVATE(logger)->interesting_level;
}

ChupaLogItemFlags
chupa_logger_get_target_item (ChupaLogger *logger)
{
    return CHUPA_LOGGER_GET_PRIVATE(logger)->target_item;
}


void
chupa_logger_set_target_item (ChupaLogger *logger,
                              ChupaLogItemFlags item)
{
    CHUPA_LOGGER_GET_PRIVATE(logger)->target_item = item;
}

void
chupa_logger_set_target_item_by_string (ChupaLogger *logger,
                                        const gchar *item_name)
{
    ChupaLogItemFlags item;

    if (item_name) {
        item = chupa_log_item_flags_from_string(item_name);
    } else {
        item = CHUPA_LOG_ITEM_DEFAULT;
    }
    chupa_logger_set_target_item(logger, item);

}

void
chupa_logger_connect_default_handler (ChupaLogger *logger)
{
    g_signal_connect(logger, "log",
                     G_CALLBACK(chupa_logger_default_log_handler), NULL);
}

void
chupa_logger_disconnect_default_handler (ChupaLogger *logger)
{
    g_signal_handlers_disconnect_by_func(
        logger, G_CALLBACK(chupa_logger_default_log_handler), NULL);
}

void
chupa_glib_log_handler (const gchar         *log_domain,
                        GLogLevelFlags       log_level,
                        const gchar         *message,
                        gpointer             user_data)
{
    ChupaLogger *logger = user_data;

    if (!logger)
        logger = chupa_logger();

#define LOG(level)                                              \
    chupa_logger_log(logger, "glib-log",                        \
                     CHUPA_LOG_LEVEL_ ## level,                 \
                     NULL, 0, NULL,                             \
                     "%s%s%s %s",                               \
                     log_domain ? "[" : "",                     \
                     log_domain ? log_domain : "",              \
                     log_domain ? "]" : "",                     \
                     message)

    switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
        LOG(ERROR);
        break;
    case G_LOG_LEVEL_CRITICAL:
        LOG(CRITICAL);
        break;
    case G_LOG_LEVEL_WARNING:
        LOG(WARNING);
        break;
    case G_LOG_LEVEL_MESSAGE:
        LOG(MESSAGE);
        break;
    case G_LOG_LEVEL_INFO:
        LOG(INFO);
        break;
    case G_LOG_LEVEL_DEBUG:
        LOG(DEBUG);
        break;
    default:
        break;
    }
#undef LOG
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
