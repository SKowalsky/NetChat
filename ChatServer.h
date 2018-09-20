//
// Created by Samuel Kowalsky on 20.09.18.
//

#ifndef NETCHAT_CHATSERVER_H
#define NETCHAT_CHATSERVER_H

#include "NetLib/UdpServer.h"
#include <map>
#include <vector>

class ChatServer : public UdpServer {
    std::map<std::string, struct Connection> clients;

    std::ostream &outstream;

    bool isregistered(struct Connection connection);
    std::string find_name(struct Connection connection);
    std::vector<std::string> extract_keys(std::map<std::string, struct Connection> const& input_map);

public:
    explicit ChatServer(const char* port) : UdpServer(port), outstream(std::cout) {}
    ChatServer(const char* port, size_t buffersize, std::ostream &outstream, std::istream &istream) : UdpServer(port, buffersize), outstream(outstream) {}

    void sendmessage(std::string message, struct Connection connection);
    void sendtoall(std::string message);
    void received(const char* msg, ssize_t msglen, struct Connection connection) override;
};

#endif //NETCHAT_CHATSERVER_H
