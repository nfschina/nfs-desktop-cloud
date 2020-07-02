#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include <stdio.h>
#include <time.h>

#define DEBUG

#ifndef DEBUG
#define LOG_INIT(logfile) ({freopen(logfile, "a", stderr); dup2(STDERR_FILENO, STDOUT_FILENO);})
#else
#define LOG_INIT(logfile)
#endif

#define LOG_MSG_LENGTH 1024

enum LOG_LEVEL{
    ERROR_LEVEL,
    WARNNIG_LEVEL,
    DEBUG_LEVEL,
    INFO_LEVEL
};

const static char *LOG_LEVEL_STRING[] = {
    "ERROR_MESSAGE",
    "WARNNING_MESSAGE",
    "DEBUG_MESSAGE",
    "INFO_MESSAGE"
};

#define LOG(level, fmt, ...) ({ \
    struct tm *tm; \
    time_t t; \
    char buf[128] = {0}; \
    char msg[LOG_MSG_LENGTH] = {0}; \
    t = time(NULL); \
    tm = localtime(&t); \
    strftime(buf, sizeof(buf), "%Y/%m/%d-%H/%M/%S", tm); \
    snprintf(msg, sizeof(msg), fmt, ##__VA_ARGS__); \
    fprintf(stderr, "[%s|%s|%s:%d|%s]\n", buf, LOG_LEVEL_STRING[level], __FILE__, __LINE__,msg); \
    })

#endif // SIMPLE_LOG_H

