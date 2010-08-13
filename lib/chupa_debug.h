#ifndef CHUPA_DEBUG_H
#define CHUPA_DEBUG_H

#ifndef CHUPA_DEBUG
#define CHUPA_DEBUG 0
#endif
#if CHUPA_DEBUG
#define CHUPA_LOG(fmt, ...) fprintf(stderr, "%s:%d:%s:" fmt "\n", \
                                    __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define CHUPA_LOG(fmt, ...) (void)0
#endif

#endif
