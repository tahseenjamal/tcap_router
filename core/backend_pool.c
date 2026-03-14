#include "backend_pool.h"

#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../config.h"

static backend_t backends[MAX_BACKENDS];

static atomic_int rr;

static int count = 0;

static int connect_backend(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("backend connect");
        return -1;
    }

    return sock;
}

void backend_pool_init() {
    atomic_store(&rr, 0);

    count = 0;

    for (int i = 0; i < 2; i++) {
        backends[i].id = i;
        backends[i].active = 1;

        atomic_store(&backends[i].load, 0);
        int sock = connect_backend("127.0.0.1", 4000 + i);

        if (sock < 0) {
            backends[i].active = 0;
            continue;
        }

        backends[i].sock = sock;
        count++;
    }
}

backend_t* backend_choose() {
    int start = atomic_fetch_add(&rr, 1);

    for (int i = 0; i < count; i++) {
        int idx = (start + i) % count;

        if (backends[idx].active) return &backends[idx];
    }

    return NULL;
}

backend_t* backend_get(int id) {
    if (id < 0 || id >= count) return NULL;

    if (!backends[id].active) return NULL;

    return &backends[id];
}
