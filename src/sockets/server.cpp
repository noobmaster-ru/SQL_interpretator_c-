#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 8080;
const int BUFFER_SIZE = 80;

class TCPServer {
private:
    int serverSocket;
    int newSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

public:
    TCPServer() {
        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "Failed to create socket." << std::endl;
            exit(EXIT_FAILURE);
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Binding failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (listen(serverSocket, 5) < 0) {
            std::cerr << "Listening failed." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void acceptConnection() {
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize)) < 0) {
            std::cerr << "Accept failed." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void receiveData() {
        int bytesReceived = read(newSocket, buffer, BUFFER_SIZE);
        if (bytesReceived < 0) {
            std::cerr << "Error in receiving data." << std::endl;
            exit(EXIT_FAILURE);
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Client: " << buffer << std::endl;

        // send to parser 
        // receive data from sql 
    }

    void sendAck() {
        const char *ack = "Message received!";
        send(newSocket, ack, strlen(ack), 0);
    }

    void closeConnection() {
        close(newSocket);
        close(serverSocket);
    }
};

int main() {
    TCPServer server;
    server.acceptConnection();
    do{
        server.receiveData();
        server.sendAck();
    }while(true);
    server.closeConnection();
    return 0;
}
