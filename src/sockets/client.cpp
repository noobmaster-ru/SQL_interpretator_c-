#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 8080;
const char *SERVER_IP = "127.0.0.1";
const int BUFFER_SIZE = 80;

class TCPClient {
private:
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

public:
    TCPClient() {
        if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "Failed to create socket." << std::endl;
            exit(EXIT_FAILURE);
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
        serverAddr.sin_port = htons(PORT);

        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Connection failed." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void sendData(){
        std::string query;
        std::getline(std::cin, query);
        
        // Copying std::string to char buffer
        if (query.size() < BUFFER_SIZE) {
            strcpy(buffer, query.c_str());
            buffer[query.size()] = '\0';
        } else {
            throw "String is too large to fit into buffer.";
        }
        
        if (send(clientSocket, buffer, strlen(buffer), 0) < 0) {
            std::cerr << "Sending failed." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void receiveAnswer() {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived < 0) {
            std::cerr << "Error in receiving data." << std::endl;
            exit(EXIT_FAILURE);
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }

    void closeConnection() {
        close(clientSocket);
    }
};

int main() {
    TCPClient client;
    do{
        try{
            client.sendData();
            client.receiveAnswer();
        }catch(const std::string& str){
            std::cerr << str << std::endl;
        }
    }while(true);
    client.closeConnection();
    return 0;
}

