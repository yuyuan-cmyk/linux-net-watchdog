#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define MAX_TARGETS 32
#define MAX_NAME_LEN 64
#define MAX_HOST_LEN 128

typedef struct {
    char name[MAX_NAME_LEN];
    char host[MAX_HOST_LEN];
    int port;
    int timeout_ms;
} monitor_target_t;

typedef struct {
    monitor_target_t targets[MAX_TARGETS];
    size_t count;
} monitor_config_t;

int load_config(const char *path, monitor_config_t *config);

#endif
