#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "192.168.0.100"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

// Function to receive messages from the server
void *receiveMessages(void *socketDescriptor) {
    int sock = *(int *)socketDescriptor;
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesRead = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            printf("Disconnected from server.\n");
            fflush(stdout); // Ensure output is flushed
            close(sock);
            exit(1);
        }
        printf("%s", buffer);
        fflush(stdout); // Ensure output is flushed
    }
    return NULL;
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char message[BUFFER_SIZE];
    pthread_t recvThread;

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection to server failed");
        close(clientSocket);
        exit(1);
    }
    //printf("Connected to chat server.\n");
    //fflush(stdout); // Ensure output is flushed

    // Start thread to receive messages
    pthread_create(&recvThread, NULL, receiveMessages, (void *)&clientSocket);
    pthread_detach(recvThread);

    // Main loop to send messages to the server
    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        if (strcmp(message, "exit\n") == 0) {
            break;
        }
        send(clientSocket, message, strlen(message), 0);
    }

    close(clientSocket);
    return 0;
}