#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

typedef struct {
    FILE *fp;
} logger_t;

int logger_open(logger_t *logger, const char *path);
void logger_close(logger_t *logger);
void logger_write(logger_t *logger, const char *level, const char *fmt, ...);

#endif
