#include "config.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

int load_config(const char *path, monitor_config_t *config)
{
    FILE *fp;
    char line[512];
    int line_no = 0;

    if (path == NULL || config == NULL) {
        return -1;
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;
    }

    config->count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        monitor_target_t target;
        int matched;

        line_no++;
        trim_line(line);

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        memset(&target, 0, sizeof(target));
        matched = sscanf(line, "%63s %127s %d %d",
                         target.name, target.host,
                         &target.port, &target.timeout_ms);

        if (matched != 4 || target.port <= 0 || target.port > 65535 ||
            target.timeout_ms <= 0) {
            fprintf(stderr, "invalid config at line %d: %s\n", line_no, line);
            fclose(fp);
            return -1;
        }

        if (config->count >= MAX_TARGETS) {
            fprintf(stderr, "too many monitor targets, max=%d\n", MAX_TARGETS);
            fclose(fp);
            return -1;
        }

        config->targets[config->count++] = target;
    }

    fclose(fp);

    if (config->count == 0) {
        fprintf(stderr, "no monitor target loaded from %s\n", path);
        return -1;
    }

    return 0;
}
