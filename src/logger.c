#include "logger.h"

#include <stdarg.h>
#include <time.h>

int logger_open(logger_t *logger, const char *path)
{
    if (logger == NULL || path == NULL) {
        return -1;
    }

    logger->fp = fopen(path, "a");
    return logger->fp == NULL ? -1 : 0;
}

void logger_close(logger_t *logger)
{
    if (logger != NULL && logger->fp != NULL) {
        fclose(logger->fp);
        logger->fp = NULL;
    }
}

void logger_write(logger_t *logger, const char *level, const char *fmt, ...)
{
    time_t now;
    struct tm tm_now;
    char ts[32];
    va_list args;

    if (logger == NULL || logger->fp == NULL || level == NULL || fmt == NULL) {
        return;
    }

    now = time(NULL);
    localtime_r(&now, &tm_now);
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_now);

    fprintf(logger->fp, "%s [%s] ", ts, level);

    va_start(args, fmt);
    vfprintf(logger->fp, fmt, args);
    va_end(args);

    fputc('\n', logger->fp);
    fflush(logger->fp);
}
