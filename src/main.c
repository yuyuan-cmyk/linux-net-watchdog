#include "config.h"
#include "logger.h"
#include "monitor.h"
#include "util.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t running = 1;

static void handle_signal(int signo)
{
    (void)signo;
    running = 0;
}

static void print_usage(const char *program)
{
    fprintf(stderr,
            "usage: %s -c <config> [-i interval_seconds] [-o log_path]\n",
            program);
}

int main(int argc, char **argv)
{
    const char *config_path = "config/watchdog.conf";
    const char *log_path = "watchdog.log";
    int interval_seconds = 5;
    monitor_config_t config;
    logger_t logger;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            if (parse_positive_int(argv[++i], &interval_seconds) != 0) {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            log_path = argv[++i];
        } else {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (load_config(config_path, &config) != 0) {
        return EXIT_FAILURE;
    }

    if (logger_open(&logger, log_path) != 0) {
        perror("open log");
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    logger_write(&logger, "INFO", "net-watchdog started, targets=%zu",
                 config.count);

    while (running) {
        size_t idx;

        for (idx = 0; idx < config.count && running; idx++) {
            const monitor_target_t *target = &config.targets[idx];
            check_result_t result = check_tcp_target(target);

            if (result.ok) {
                logger_write(&logger, "INFO", "%s %s:%d OK latency=%ldms",
                             target->name, target->host, target->port,
                             result.latency_ms);
                printf("[OK] %s %s:%d latency=%ldms\n",
                       target->name, target->host, target->port,
                       result.latency_ms);
            } else {
                logger_write(&logger, "WARN", "%s %s:%d FAIL %s",
                             target->name, target->host, target->port,
                             result.message);
                printf("[FAIL] %s %s:%d %s\n",
                       target->name, target->host, target->port,
                       result.message);
            }
        }

        sleep_seconds_interruptible(interval_seconds, &running);
    }

    logger_write(&logger, "INFO", "net-watchdog stopped");
    logger_close(&logger);
    return EXIT_SUCCESS;
}
