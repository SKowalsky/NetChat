//
// Created by Samuel Kowalsky on 20.09.18.
//

#include "UdpClient.h"

void* UdpClient::get_in_addr(struct sockaddr *addr) {
    if(addr->sa_family == AF_INET) return &(((struct sockaddr_in*)addr)->sin_addr); //ipv4
    return &(((struct sockaddr_in6*)addr)->sin6_addr);  //ipv6
}

bool UdpClient::start() {
    struct addrinfo hints = {}, *res = nullptr, *p = nullptr;   //initialize structures
    int sockd = -1;

    memset(&hints, 0, sizeof hints);    //make sure it's empty
    hints.ai_family = AF_UNSPEC;    //ipv4 and ipv6
    hints.ai_socktype = SOCK_DGRAM; //datagram sockets (udp)

    if(getaddrinfo(address, port, &hints, &res) != 0) { //fill res structure with host information
        //didn't work
        perror("Failed to get address information");
        return false;
    }

    for(p = res; p != nullptr; p = p->ai_next) {    //loop through all results
        if((sockd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {  //create socket
            //didn't work
            continue;   //try next linked list element
        }
        //successfully created socket
        conn.sockd = sockd;
        conn.addr = *p->ai_addr;
        conn.addrlen = p->ai_addrlen;
        break;
    }

    freeaddrinfo(res);  //free memory
    if(p == nullptr) return false;  //no result worked

    running = true;
    recv_thread = std::thread(&UdpClient::trecv, this);   //start receive thread

    return true;
}

bool UdpClient::stop() {
    running = false;
    return shutdown(conn.sockd, 2) == 0;    //close socket
}

void UdpClient::trecv() {
    ssize_t read = 0;   //actually read bytes
    char msg[buffersize];   //message buffer
    while(running) {
        struct Connection connection = {conn.sockd};
        if((read = recvfrom(conn.sockd, msg, buffersize, 0, &conn.addr, &conn.addrlen) == -1)) {    //receive message
            perror("Failed to receive message");
            stop(); //stop client
        }
        if(read <= 0) continue; //nothing to read
        received(msg, read, connection);    //call override function
    }
}

void UdpClient::send(const char *msg, size_t msglen) {
    sendto(conn.sockd, msg, msglen, 0, &conn.addr, conn.addrlen);   //send message to client
}

std::string UdpClient::get_address(struct Connection connection) {
    char address[INET6_ADDRSTRLEN];
    inet_ntop(connection.addr.sa_family, get_in_addr(&connection.addr), address, sizeof address);
    return std::string(address);
}