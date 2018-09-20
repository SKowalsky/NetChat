//
// Created by Samuel Kowalsky on 20.09.18.
//

#include "UdpServer.h"

void* get_in_addr(struct sockaddr *addr) {
    if(addr->sa_family == AF_INET) return &(((struct sockaddr_in*)addr)->sin_addr); //ipv4
    return &(((struct sockaddr_in6*)addr)->sin6_addr);  //ipv6
}

bool UdpServer::start() {
    struct addrinfo hints = {}, *res = nullptr, *p = nullptr;   //initialize structures
    int sockd = -1;

    memset(&hints, 0, sizeof hints);    //make sure it's empty
    hints.ai_family = AF_UNSPEC;    //ipv4 and ipv6
    hints.ai_socktype = SOCK_DGRAM; //datagram sockets (udp)
    hints.ai_flags = AI_PASSIVE;    //use localhost

    if(getaddrinfo(nullptr, port, &hints, &res) != 0) { //fill res structure with host information
        //didn't work
        perror("Failed to get address information");
        return false;
    }

    for(p = res; p != nullptr; p = p->ai_next) {    //loop through all results
        if((sockd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {  //create socket
            //didn't work
            continue;   //try next linked list element
        }
        if(bind(sockd, p->ai_addr, p->ai_addrlen) == -1) {  //bind socket
            //didn't work
            continue;   //try next linked list element
        }
        //successfully bound socket
        conn.sockd = sockd;
        conn.addr = *p->ai_addr;
        conn.addrlen = p->ai_addrlen;
        break;
    }

    freeaddrinfo(res);  //free memory
    if(p == nullptr) return false;  //no result worked

    running = true;
    recv_thread = std::thread(&UdpServer::trecv, this);   //start receive thread

    return true;
}

bool UdpServer::stop() {
    running = false;
    return shutdown(conn.sockd, 2) == 0;    //close socket
}

void UdpServer::trecv() {
    ssize_t read = 0;   //actually read bytes
    char msg[buffersize];   //message buffer
    while(running) {
        struct Connection connection = {conn.sockd};
        if((read = recvfrom(conn.sockd, msg, buffersize, 0, &conn.addr, &conn.addrlen) == -1)) {    //receive message
            perror("Failed to receive message");
            stop(); //stop server
        }
        if(read <= 0) continue; //nothing to read
        received(msg, read, connection);    //call override function
    }
}

void UdpServer::send(const char *msg, size_t msglen, struct Connection connection) {
    sendto(conn.sockd, msg, msglen, 0, &connection.addr, connection.addrlen);   //send message to client
}

std::string UdpServer::get_address(struct Connection connection) {
    char address[INET6_ADDRSTRLEN];
    inet_ntop(connection.addr.sa_family, get_in_addr(&connection.addr), address, sizeof address);
    return std::string(address);
}