
#if !defined(DIKE_DEBUG_HPP)
#define DIKE_DEBUG_HPP

#if !defined(DIKE_DEBUG_NAME)
#define DIKE_DEBUG_NAME                 "unknown"
#endif

enum {
        DIKE_DEBUG_LEVEL_INVALID        = 0,
        DIKE_DEBUG_LEVEL_SILENT         = 1,
        DIKE_DEBUG_LEVEL_ERROR          = 2,
        DIKE_DEBUG_LEVEL_WARNING        = 3,
        DIKE_DEBUG_LEVEL_NOTICE         = 4,
        DIKE_DEBUG_LEVEL_INFO           = 5,
        DIKE_DEBUG_LEVEL_DEBUG          = 6,
        DIKE_DEBUG_LEVEL_TRACE          = 7
#define DIKE_DEBUG_LEVEL_INVALID        DIKE_DEBUG_LEVEL_INVALID
#define DIKE_DEBUG_LEVEL_SILENT         DIKE_DEBUG_LEVEL_SILENT
#define DIKE_DEBUG_LEVEL_ERROR          DIKE_DEBUG_LEVEL_ERROR
#define DIKE_DEBUG_LEVEL_WARNING        DIKE_DEBUG_LEVEL_WARNING
#define DIKE_DEBUG_LEVEL_NOTICE         DIKE_DEBUG_LEVEL_NOTICE
#define DIKE_DEBUG_LEVEL_INFO           DIKE_DEBUG_LEVEL_INFO
#define DIKE_DEBUG_LEVEL_DEBUG          DIKE_DEBUG_LEVEL_DEBUG
#define DIKE_DEBUG_LEVEL_TRACE          DIKE_DEBUG_LEVEL_TRACE
};

extern unsigned int dikeDebugLevel;

#define dikeEnterf() { \
        dikeTracef("enter"); \
}

#define dike_leavef() { \
        dikeTracef("leave"); \
}

#define dikeTracef(a...) { \
        if (dikeDebugLevel >= DIKE_DEBUG_LEVEL_TRACE) { \
                DikeDebugPrintf(DIKE_DEBUG_LEVEL_TRACE, DIKE_DEBUG_NAME, __FUNCTION__, __FILE__, __LINE__, a); \
        } \
}

#define dikeDebugf(a...) { \
        if (dikeDebugLevel >= DIKE_DEBUG_LEVEL_DEBUG) { \
                DikeDebugPrintf(DIKE_DEBUG_LEVEL_DEBUG, DIKE_DEBUG_NAME, __FUNCTION__, __FILE__, __LINE__, a); \
        } \
}

#define dikeWarningf(a...) { \
        if (dikeDebugLevel >= DIKE_DEBUG_LEVEL_WARNING) { \
                DikeDebugPrintf(DIKE_DEBUG_LEVEL_WARNING, DIKE_DEBUG_NAME, __FUNCTION__, __FILE__, __LINE__, a); \
        } \
}

#define dikeNoticef(a...) { \
        if (dikeDebugLevel >= DIKE_DEBUG_LEVEL_NOTICE) { \
                DikeDebugPrintf(DIKE_DEBUG_LEVEL_NOTICE, DIKE_DEBUG_NAME, __FUNCTION__, __FILE__, __LINE__, a); \
        } \
}

#define dikeInfof(a...) { \
        if (dikeDebugLevel >= DIKE_DEBUG_LEVEL_INFO) { \
                DikeDebugPrintf(DIKE_DEBUG_LEVEL_INFO, DIKE_DEBUG_NAME, __FUNCTION__, __FILE__, __LINE__, a); \
        } \
}

#define dikeErrorf(a...) { \
        if (dikeDebugLevel >= DIKE_DEBUG_LEVEL_ERROR) { \
                DikeDebugPrintf(DIKE_DEBUG_LEVEL_ERROR, DIKE_DEBUG_NAME, __FUNCTION__, __FILE__, __LINE__, a); \
        } \
}

int DikeDebugInit (void);
void DikeDebugFini (void);

const char * DikeDebugLevelToString (unsigned int level);
unsigned int DikeDebugLevelFromString (const char *string);
int DikeDebugSetLevel (unsigned int level);
int DikeDebugSetLevel (const char *level);
int DikeDebugPrintf (unsigned int level, const char *name, const char *function, const char *file, int line, const char *fmt, ...) __attribute__((format(printf, 6, 7)));

void DikeDebugLock (void);
void DikeDebugUnlock (void);

#endif
