#ifndef MONITOR_H
#define MONITOR_H

#include "config.h"

typedef struct {
    int ok;
    long latency_ms;
    char message[128];
} check_result_t;

check_result_t check_tcp_target(const monitor_target_t *target);

#endif
