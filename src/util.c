#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

long monotonic_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

void trim_line(char *line)
{
    char *start;
    char *end;

    if (line == NULL) {
        return;
    }

    start = line;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != line) {
        memmove(line, start, strlen(start) + 1);
    }

    end = line + strlen(line);
    while (end > line && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';
}

int parse_positive_int(const char *text, int *value)
{
    char *end = NULL;
    long parsed;

    if (text == NULL || value == NULL) {
        return -1;
    }

    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed <= 0 ||
        parsed > 2147483647L) {
        return -1;
    }

    *value = (int)parsed;
    return 0;
}

void sleep_seconds_interruptible(int seconds, volatile sig_atomic_t *running)
{
    int i;

    for (i = 0; i < seconds; i++) {
        if (running != NULL && !*running) {
            return;
        }
        sleep(1);
    }
}
