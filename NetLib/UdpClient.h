//
// Created by Samuel Kowalsky on 20.09.18.
//

#ifndef NETCHAT_UDPCLIENT_H
#define NETCHAT_UDPCLIENT_H

#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <thread>
#include "Connection.h"

class UdpClient {
protected:
    const char* address;
    const char* port;
    struct Connection conn;
    size_t buffersize;
    bool running;

    std::thread recv_thread;
    void trecv();

    void* get_in_addr(struct sockaddr *addr);

public:
    explicit UdpClient(const char* address, const char* port) : address(address), port(port), running(false), conn({}), buffersize(1024) {}
    UdpClient(const char* address, const char* port, size_t buffersize) : address(address), port(port), running(false), conn({}), buffersize(buffersize) {}
    bool start();
    bool stop();

    std::string get_address(struct Connection connection);
    bool isrunning() const { return running; }

    void send(const char* msg, size_t msglen);
    virtual void received(const char* msg, ssize_t msglen, struct Connection connection) = 0;   //pure virtual (must be overwritten)
};

#endif //NETCHAT_UDPCLIENT_H