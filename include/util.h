#ifndef UTIL_H
#define UTIL_H

#include <signal.h>
#include <stddef.h>

long monotonic_ms(void);
void trim_line(char *line);
int parse_positive_int(const char *text, int *value);
void sleep_seconds_interruptible(int seconds, volatile sig_atomic_t *running);

#endif
