//
// Created by Samuel Kowalsky on 20.09.18.
//

#ifndef NETCHAT_CONNECTION_H
#define NETCHAT_CONNECTION_H

#include <netdb.h>
#include <string>

struct Connection {
    int sockd;
    struct sockaddr addr;
    socklen_t addrlen;
};

#endif //NETCHAT_CONNECTION_H
