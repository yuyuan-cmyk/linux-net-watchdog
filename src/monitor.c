#include "monitor.h"
#include "util.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void set_message(check_result_t *result, const char *message)
{
    snprintf(result->message, sizeof(result->message), "%s", message);
}

check_result_t check_tcp_target(const monitor_target_t *target)
{
    check_result_t result;
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *rp;
    char port_text[16];
    long start_ms;
    int gai_ret;

    memset(&result, 0, sizeof(result));
    result.latency_ms = -1;

    if (target == NULL) {
        set_message(&result, "invalid target");
        return result;
    }

    snprintf(port_text, sizeof(port_text), "%d", target->port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    gai_ret = getaddrinfo(target->host, port_text, &hints, &res);
    if (gai_ret != 0) {
        snprintf(result.message, sizeof(result.message), "resolve error: %s",
                 gai_strerror(gai_ret));
        return result;
    }

    start_ms = monotonic_ms();

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int fd;
        int flags;
        int ret;
        int socket_error = 0;
        socklen_t len = sizeof(socket_error);
        struct pollfd pfd;

        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) {
            continue;
        }

        flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            close(fd);
            continue;
        }

        ret = connect(fd, rp->ai_addr, rp->ai_addrlen);
        if (ret == 0) {
            result.ok = 1;
            result.latency_ms = monotonic_ms() - start_ms;
            set_message(&result, "connected");
            close(fd);
            break;
        }

        if (errno != EINPROGRESS) {
            snprintf(result.message, sizeof(result.message), "connect error: %s",
                     strerror(errno));
            close(fd);
            continue;
        }

        pfd.fd = fd;
        pfd.events = POLLOUT;
        pfd.revents = 0;

        ret = poll(&pfd, 1, target->timeout_ms);
        if (ret > 0 &&
            getsockopt(fd, SOL_SOCKET, SO_ERROR, &socket_error, &len) == 0 &&
            socket_error == 0) {
            result.ok = 1;
            result.latency_ms = monotonic_ms() - start_ms;
            set_message(&result, "connected");
            close(fd);
            break;
        }

        if (ret == 0) {
            snprintf(result.message, sizeof(result.message),
                     "timeout after %dms", target->timeout_ms);
        } else if (socket_error != 0) {
            snprintf(result.message, sizeof(result.message), "connect error: %s",
                     strerror(socket_error));
        } else {
            snprintf(result.message, sizeof(result.message), "poll error: %s",
                     strerror(errno));
        }

        close(fd);
    }

    freeaddrinfo(res);

    if (!result.ok && result.message[0] == '\0') {
        set_message(&result, "no reachable address");
    }

    return result;
}
