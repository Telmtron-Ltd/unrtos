
#ifndef XPRINT_H
#define XPRINT_H

#include <stdio.h>

#define XLOG_NONE  (0)
#define XLOG_RAW   (1)
#define XLOG_ERR   (2)
#define XLOG_WARN  (3)
#define XLOG_INFO  (4)
#define XLOG_DEBUG (5)

/* Default log level if XLOG_LEVEL is not defined */
#ifndef XLOG_LEVEL
#define XLOG_LEVEL XLOG_INFO
#warning Using default log level of XLOG_INFO. To use a different level, define XLOG_LEVEL before including xprint.h
#endif

#if XLOG_LEVEL == XLOG_NONE
#warning Logging disabled
#endif

#if XLOG_LEVEL >= XLOG_RAW
#define _xprint_RAW(...)     do{ printf("\033[37m"); printf(__VA_ARGS__);  printf("\033[0m"); } while(0)
#define _xprintln_RAW(...)   do{ _xprint_RAW(__VA_ARGS__);  putchar('\n'); } while(0)
#else
#define _xprint_RAW(...)     XLOG_LEVEL_DISABLED("RAW");
#define _xprintln_RAW(...)   XLOG_LEVEL_DISABLED("RAW");
#endif

#if XLOG_LEVEL >= XLOG_ERR
#define _xprint_ERR(...)     do{ printf("\033[31m[ERROR] %s: ", __func__); printf(__VA_ARGS__); printf("\033[0m"); } while(0)
#define _xprintln_ERR(...)   do{ _xprint_ERR(__VA_ARGS__);  putchar('\n'); } while(0)
#else
#define _xprint_ERR(...)     XLOG_LEVEL_DISABLED("ERR");
#define _xprintln_ERR(...)   XLOG_LEVEL_DISABLED("ERR");
#endif

#if XLOG_LEVEL >= XLOG_WARN
#define _xprint_WARN(...)    do{ printf("\033[33m[WARNING] %s: ", __func__); printf(__VA_ARGS__); printf("\033[0m"); } while(0)
#define _xprintln_WARN(...)  do{ _xprint_WARN(__VA_ARGS__);  putchar('\n'); } while(0)
#else
#define _xprint_WARN(...)    XLOG_LEVEL_DISABLED("WARN");
#define _xprintln_WARN(...)  XLOG_LEVEL_DISABLED("WARN");
#endif

#if XLOG_LEVEL >= XLOG_INFO
#define _xprint_INFO(...)    do{ printf("\033[32m[INFO] %s: ", __func__); printf(__VA_ARGS__); printf("\033[0m"); } while(0)
#define _xprintln_INFO(...)  do{ _xprint_INFO(__VA_ARGS__);  putchar('\n'); } while(0)
#else
#define _xprint_INFO(...)    XLOG_LEVEL_DISABLED("INFO");
#define _xprintln_INFO(...)  XLOG_LEVEL_DISABLED("INFO");
#endif

#if XLOG_LEVEL >= XLOG_DEBUG
#define _xprint_DEBUG(...)   do{ printf("\033[36m[DEBUG] %s: ", __func__); printf(__VA_ARGS__); printf("\033[0m"); } while(0)
#define _xprintln_DEBUG(...) do{ _xprint_DEBUG(__VA_ARGS__);  putchar('\n'); } while(0)
#else
#define _xprint_DEBUG(...)   XLOG_LEVEL_DISABLED("DEBUG");
#define _xprintln_DEBUG(...) XLOG_LEVEL_DISABLED("DEBUG");
#endif

#define _xprint_OFF(...)   XLOG_LEVEL_DISABLED("DEBUG");
#define _xprintln_OFF(...) XLOG_LEVEL_DISABLED("DEBUG");


#define xprint(lvl, ...)    _xprint_ ## lvl (__VA_ARGS__)
#define xprintln(lvl, ...)  _xprintln_ ## lvl (__VA_ARGS__)

/* Catch function if the log level is disabled, for pretty intellisense */
static inline void XLOG_LEVEL_DISABLED(void* level) {}

#endif /* XPRINT_H */
