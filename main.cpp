#include <iostream>
#include "ChatServer.h"

int main() {

    auto *server = new ChatServer("1024", 1024, std::cout, std::cin);   //port, buffer size, output stream, input stream

    std::string input;
    if(!server->start()) {
        std::cout << "Failed to start server";
        exit(1);
    }

    std::cout << "Server now running\n";
    while(server->isrunning()) {
        std::getline(std::cin, input);
        if(input == "stop") {
            server->stop();
            std::cout << "Server stopped\n";
        }
    }
    std::cout << "Exiting application...\n";

    return 0;
}