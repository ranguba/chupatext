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

#ifndef __CHUPA_LOGGER_H__
#define __CHUPA_LOGGER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define chupa_log(level, format, ...)                           \
    do {                                                        \
        if (chupa_need_log(level)) {                            \
            (chupa_logger_log(chupa_logger(),                   \
                              CHUPA_LOG_DOMAIN,                 \
                              (level),                          \
                              __FILE__,                         \
                              __LINE__,                         \
                              G_STRFUNC,                        \
                              format, ## __VA_ARGS__));         \
        }                                                       \
    } while (0)

#define chupa_critical(format, ...)                             \
    chupa_log(CHUPA_LOG_LEVEL_CRITICAL, format, ## __VA_ARGS__)
#define chupa_error(format, ...)                                \
    chupa_log(CHUPA_LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define chupa_warning(format, ...)                              \
    chupa_log(CHUPA_LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define chupa_message(format, ...)                              \
    chupa_log(CHUPA_LOG_LEVEL_MESSAGE, format, ## __VA_ARGS__)
#define chupa_info(format, ...)                                 \
    chupa_log(CHUPA_LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define chupa_debug(format, ...)                                \
    chupa_log(CHUPA_LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
#define chupa_statistics(format, ...)                                  \
    chupa_log(CHUPA_LOG_LEVEL_STATISTICS, format, ## __VA_ARGS__)
#define chupa_profile(format, ...)                              \
    chupa_log(CHUPA_LOG_LEVEL_PROFILE, format, ## __VA_ARGS__)

#define chupa_set_log_level(level)                              \
    chupa_logger_set_target_level(chupa_logger(), (level))
#define chupa_get_log_level()                                   \
    chupa_logger_get_resolved_target_level(chupa_logger())
#define chupa_get_interesting_log_level()               \
    chupa_logger_get_interesting_level(chupa_logger())

#define chupa_need_log(level)                           \
    (chupa_get_interesting_log_level() & (level))
#define chupa_need_critical_log()               \
    (chupa_need_log(CHUPA_LOG_LEVEL_CRITICAL))
#define chupa_need_error_log()                  \
    (chupa_need_log(CHUPA_LOG_LEVEL_ERROR))
#define chupa_need_warning_log()                \
    (chupa_need_log(CHUPA_LOG_LEVEL_WARNING))
#define chupa_need_message_log()                \
    (chupa_need_log(CHUPA_LOG_LEVEL_MESSAGE))
#define chupa_need_info_log()                   \
    (chupa_need_log(CHUPA_LOG_LEVEL_INFO))
#define chupa_need_debug_log()                  \
    (chupa_need_log(CHUPA_LOG_LEVEL_DEBUG))
#define chupa_need_statistics_log()                     \
    (chupa_need_log(CHUPA_LOG_LEVEL_STATISTICS))
#define chupa_need_profile_log()                \
    (chupa_need_log(CHUPA_LOG_LEVEL_PROFILE))

#define CHUPA_TYPE_LOGGER            (chupa_logger_get_type())
#define CHUPA_LOGGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CHUPA_TYPE_LOGGER, ChupaLogger))
#define CHUPA_LOGGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), CHUPA_TYPE_LOGGER, ChupaLoggerClass))
#define CHUPA_IS_LOGGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CHUPA_TYPE_LOGGER))
#define CHUPA_IS_LOGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), CHUPA_TYPE_LOGGER))
#define CHUPA_LOGGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_LOGGER, ChupaLoggerClass))

typedef enum
{
    CHUPA_LOG_LEVEL_DEFAULT    = 0,
    CHUPA_LOG_LEVEL_NONE       = 1 << 0,
    CHUPA_LOG_LEVEL_CRITICAL   = 1 << 1,
    CHUPA_LOG_LEVEL_ERROR      = 1 << 2,
    CHUPA_LOG_LEVEL_WARNING    = 1 << 3,
    CHUPA_LOG_LEVEL_MESSAGE    = 1 << 4,
    CHUPA_LOG_LEVEL_INFO       = 1 << 5,
    CHUPA_LOG_LEVEL_DEBUG      = 1 << 6,
    CHUPA_LOG_LEVEL_STATISTICS = 1 << 7,
    CHUPA_LOG_LEVEL_PROFILE    = 1 << 8
} ChupaLogLevelFlags;

#define CHUPA_LOG_LEVEL_ALL (CHUPA_LOG_LEVEL_CRITICAL |        \
                             CHUPA_LOG_LEVEL_ERROR |           \
                             CHUPA_LOG_LEVEL_WARNING |         \
                             CHUPA_LOG_LEVEL_MESSAGE |         \
                             CHUPA_LOG_LEVEL_INFO |            \
                             CHUPA_LOG_LEVEL_DEBUG |           \
                             CHUPA_LOG_LEVEL_STATISTICS |      \
                             CHUPA_LOG_LEVEL_PROFILE)

typedef enum
{
    CHUPA_LOG_ITEM_DEFAULT     = 0,
    CHUPA_LOG_ITEM_NONE        = 1 << 0,
    CHUPA_LOG_ITEM_DOMAIN      = 1 << 1,
    CHUPA_LOG_ITEM_LEVEL       = 1 << 2,
    CHUPA_LOG_ITEM_LOCATION    = 1 << 3,
    CHUPA_LOG_ITEM_TIME        = 1 << 4
} ChupaLogItemFlags;

#define CHUPA_LOG_ITEM_ALL (CHUPA_LOG_ITEM_DOMAIN |    \
                            CHUPA_LOG_ITEM_LEVEL |     \
                            CHUPA_LOG_ITEM_LOCATION |  \
                            CHUPA_LOG_ITEM_TIME)

typedef enum
{
    CHUPA_LOG_COLORIZE_DEFAULT,
    CHUPA_LOG_COLORIZE_NONE,
    CHUPA_LOG_COLORIZE_CONSOLE
} ChupaLogColorize;

typedef struct _ChupaLogger         ChupaLogger;
typedef struct _ChupaLoggerClass    ChupaLoggerClass;

struct _ChupaLogger
{
    GObject object;
};

struct _ChupaLoggerClass
{
    GObjectClass parent_class;

    void (*log) (ChupaLogger        *logger,
                 const gchar         *domain,
                 ChupaLogLevelFlags  level,
                 const gchar         *file,
                 guint                line,
                 const gchar         *function,
                 GTimeVal            *time_value,
                 const gchar         *message);
};

GQuark           chupa_logger_error_quark    (void);

GType            chupa_logger_get_type       (void) G_GNUC_CONST;

ChupaLogLevelFlags chupa_log_level_flags_from_string (const gchar *level_name);
ChupaLogItemFlags  chupa_log_item_flags_from_string  (const gchar *item_name);

ChupaLogger     *chupa_logger                (void);
void             chupa_logger_default_log_handler
                                             (ChupaLogger         *logger,
                                              const gchar         *domain,
                                              ChupaLogLevelFlags   level,
                                              const gchar         *file,
                                              guint                line,
                                              const gchar         *function,
                                              GTimeVal            *time_value,
                                              const gchar         *message,
                                              gpointer             user_data);
ChupaLogger     *chupa_logger_new            (void);
void             chupa_logger_log            (ChupaLogger         *logger,
                                              const gchar         *domain,
                                              ChupaLogLevelFlags   level,
                                              const gchar         *file,
                                              guint                line,
                                              const gchar         *function,
                                              const gchar         *format,
                                              ...) G_GNUC_PRINTF(7, 8);
void             chupa_logger_log_va_list    (ChupaLogger         *logger,
                                              const gchar         *domain,
                                              ChupaLogLevelFlags   level,
                                              const gchar         *file,
                                              guint                line,
                                              const gchar         *function,
                                              const gchar         *format,
                                              va_list              args);
ChupaLogLevelFlags
                 chupa_logger_get_target_level(ChupaLogger        *logger);
ChupaLogLevelFlags
                 chupa_logger_get_resolved_target_level
                                              (ChupaLogger        *logger);
void             chupa_logger_set_target_level(ChupaLogger        *logger,
                                               ChupaLogLevelFlags  level);
void             chupa_logger_set_target_level_by_string
                                              (ChupaLogger        *logger,
                                               const gchar        *level_name);
void             chupa_logger_set_interesting_level
                                              (ChupaLogger        *logger,
                                               const gchar        *key,
                                               ChupaLogLevelFlags  level);
ChupaLogLevelFlags
                 chupa_logger_get_interesting_level
                                              (ChupaLogger        *logger);
ChupaLogItemFlags
                 chupa_logger_get_target_item (ChupaLogger        *logger);
void             chupa_logger_set_target_item (ChupaLogger        *logger,
                                               ChupaLogItemFlags   item);
void             chupa_logger_set_target_item_by_string
                                              (ChupaLogger        *logger,
                                               const gchar        *item_name);

void             chupa_logger_connect_default_handler
                                              (ChupaLogger        *logger);
void             chupa_logger_disconnect_default_handler
                                              (ChupaLogger        *logger);

#define CHUPA_GLIB_LOG_DELEGATE(domain)         \
    g_log_set_handler(domain,                   \
                      G_LOG_LEVEL_MASK |        \
                      G_LOG_FLAG_RECURSION |    \
                      G_LOG_FLAG_FATAL,         \
                      chupa_glib_log_handler,   \
                      NULL)

void             chupa_glib_log_handler       (const gchar         *log_domain,
                                               GLogLevelFlags       log_level,
                                               const gchar         *message,
                                               gpointer             user_data);

G_END_DECLS

#endif /* __CHUPA_LOGGER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
