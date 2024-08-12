
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define DIKE_DEBUG_NAME                 "DikeDebug"
#include "DikeDebug.hpp"

unsigned int dikeDebugLevel             = DIKE_DEBUG_LEVEL_INFO;
static int debug_initialized            = 0;
static char *debug_buffer               = NULL;
static int debug_buffer_size            = 0;

int DikeDebugInit (void)
{
        if (debug_initialized) {
                return 0;
        }

        debug_buffer      = NULL;
        debug_buffer_size = 0;
        debug_initialized = 1;
        return 0;
}

void DikeDebugFini (void)
{
        if (debug_buffer != NULL) {
                free(debug_buffer);
        }
        debug_buffer      = NULL;
        debug_buffer_size = 0;
        debug_initialized = 0;
}

const char * DikeDebugLevelToString (unsigned int level)
{
        switch (level) {
                case DIKE_DEBUG_LEVEL_SILENT:  return "silent";
                case DIKE_DEBUG_LEVEL_ERROR:   return "error";
                case DIKE_DEBUG_LEVEL_WARNING: return "warning";
                case DIKE_DEBUG_LEVEL_NOTICE:  return "notice";
                case DIKE_DEBUG_LEVEL_INFO:    return "info";
                case DIKE_DEBUG_LEVEL_DEBUG:   return "debug";
                case DIKE_DEBUG_LEVEL_TRACE:   return "trace";
        }
        return "invalid";
}

unsigned int DikeDebugLevelFromString (const char *string)
{
        if (string == NULL) {
                return DIKE_DEBUG_LEVEL_INVALID;
        }
        if (strcmp(string, "silent") == 0 || strcmp(string, "s") == 0) {
                return DIKE_DEBUG_LEVEL_SILENT;
        }
        if (strcmp(string, "error") == 0 || strcmp(string, "e") == 0) {
                return DIKE_DEBUG_LEVEL_ERROR;
        }
        if (strcmp(string, "warning") == 0 || strcmp(string, "w") == 0) {
                return DIKE_DEBUG_LEVEL_WARNING;
        }
        if (strcmp(string, "notice") == 0 || strcmp(string, "n") == 0) {
                return DIKE_DEBUG_LEVEL_NOTICE;
        }
        if (strcmp(string, "info") == 0 || strcmp(string, "i") == 0) {
                return DIKE_DEBUG_LEVEL_INFO;
        }
        if (strcmp(string, "debug") == 0 || strcmp(string, "d") == 0) {
                return DIKE_DEBUG_LEVEL_DEBUG;
        }
        if (strcmp(string, "trace") == 0 || strcmp(string, "t") == 0) {
                return DIKE_DEBUG_LEVEL_TRACE;
        }
        return DIKE_DEBUG_LEVEL_INVALID;
}

int DikeDebugSetLevel (unsigned int level)
{
        dikeDebugLevel = level;
        return dikeDebugLevel;
}

int DikeDebugSetLevel (const char *level)
{
        return DikeDebugSetLevel(DikeDebugLevelFromString(level));
}

int DikeDebugPrintf (unsigned int level, const char *name, const char *function, const char *file, int line, const char *fmt, ...)
{
        int rc;
        va_list ap;

        time_t timeval_tv_sec;
        struct timeval timeval;
        struct tm *tm;
        int milliseconds;
        char date[80];

        DikeDebugInit();

        va_start(ap, fmt);
        rc = vsnprintf(debug_buffer, debug_buffer_size, fmt, ap);
        va_end(ap);
        if (rc < 0) {
                goto bail;
        }
        if (debug_buffer_size == 0 ||
            rc >= debug_buffer_size) {
                free(debug_buffer);
                debug_buffer = (char *) malloc(rc + 1);
                if (debug_buffer == NULL) {
                        goto bail;
                }
                debug_buffer_size = rc + 1;
                va_start(ap, fmt);
                rc = vsnprintf(debug_buffer, debug_buffer_size, fmt, ap);
                va_end(ap);
                if (rc < 0) {
                        goto bail;
                }
        }

        gettimeofday(&timeval, NULL);

        milliseconds = (int) ((timeval.tv_usec / 1000.0) + 0.5);
        if (milliseconds >= 1000) {
                milliseconds -= 1000;
                timeval.tv_sec++;
        }
        timeval_tv_sec = timeval.tv_sec;
        tm = localtime(&timeval_tv_sec);
        strftime(date, sizeof(date), "%x %H:%M:%S", tm);

        fprintf(stderr, "dike:%s.%03d:%-5s:%-5s: %s (%s %s:%d)\n", date, milliseconds, name, DikeDebugLevelToString(level), debug_buffer, function, file, line);
        fflush(stderr);

        return 0;
bail:   va_end(ap);
        return -1;
}
