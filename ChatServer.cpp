//
// Created by Samuel Kowalsky on 20.09.18.
//

#include "ChatServer.h"

void ChatServer::received(const char *msg, ssize_t msglen, struct Connection connection) {
    std::string message(msg, (unsigned long)msglen);   //convert message to string
    std::string prefix = message.substr(0,3);   //get first 3 characters
    std::string text = message.substr(3, message.find_first_of("/e/")-3);   //get text between prefix and /e/

    if(prefix.size() == 0 || text.size() == 0) return;  //wrong message format

    if(prefix == "/c/") {   //connection request
        if(isregistered(connection)) return;    //already registered, ignore

        //append number to name, if a client with the name already exists
        std::string textcpy = text;
        int number = 2;
        while(clients.find(textcpy) == clients.end()) textcpy = text + std::to_string(number++); //find new name if neccessary
        text = textcpy; //change name to new name

        clients[text] = connection; //add client to list

        std::string printable_message = text+" connected!";
        sendtoall("/c/"+printable_message+"/e/"); //send connection response
        outstream << printable_message << std::endl;    //print to console
    } else if(prefix == "/d/") {    //disconnection request
        if(!isregistered(connection)) return;    //not registered, ignore

        std::string name = find_name(connection);   //get name
        if(name.empty()) return;    //should not happen

        std::string printable_message = name+" disconnected!";
        sendtoall("/d/"+printable_message+"/e/");    //broadcast disconnection message
        outstream << printable_message << std::endl;    //print to console

        clients.erase(name);    //delete client from client list
    } else if(prefix == "/m/") {    //message
        if(!isregistered(connection)) return;   //not registered, ignore

        std::string name = find_name(connection);   //get name
        if(name.empty()) return;    //should not happen

        std::string printable_message = name+": "+text;
        sendtoall("/m/"+printable_message+"/e/");  //broadcast message
        outstream << printable_message << std::endl;    //print to console
    } else if(prefix == "/p/") {    //ping pong timeout message
        //TODO implement timeout
    }
}

void ChatServer::sendmessage(std::string message, struct Connection connection) {
    send(message.c_str(), message.size(), connection);  //send string message to client
}

void ChatServer::sendtoall(std::string message) {
    for(auto &client : clients) sendmessage(message, client.second);    //loop through all clients
}

bool ChatServer::isregistered(struct Connection connection) {
    for(auto &client : clients) if(client.second.addr.sa_data == connection.addr.sa_data) return true;  //check if client exists
    return false;
}

std::string ChatServer::find_name(struct Connection connection) {
    std::vector<std::string> keys = extract_keys(clients);
    for(std::string key : keys) if(clients[key].addr.sa_data == connection.addr.sa_data) return key;
    return "";
}

std::vector<std::string> ChatServer::extract_keys(std::map<std::string, struct Connection> const &input_map) {
    std::vector<std::string> retval;
    for (auto const& element : input_map) {
        retval.push_back(element.first);
    }
    return retval;
}
