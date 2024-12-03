#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Structure for client info
typedef struct {
    int socket;
    char name[30];
    int assignedNumber;  // Store the sequential number assigned to the client
} Client;

// Global client list
Client *clientList[MAX_CLIENTS];
int clientCount = 0;
pthread_mutex_t clientListLock = PTHREAD_MUTEX_INITIALIZER;

// Broadcast the client list to a specific client
void ClientList(int clientSocket) {
    pthread_mutex_lock(&clientListLock);
    char clientListMessage[BUFFER_SIZE] = "Connected clients:\n";
    pthread_mutex_unlock(&clientListLock);
}

// Notify all clients about a status change
void broadcastStatus(const char *message) {
    pthread_mutex_lock(&clientListLock);
    for (int i = 0; i < clientCount; i++) {
        (clientList[i]->socket, message, strlen(message), 0);
    }
    pthread_mutex_unlock(&clientListLock);
}

// Client handler function
void *clientHandler(void *arg) {
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];

    // Ask for client's name
    recv(client->socket, client->name, sizeof(client->name), 0);
    client->name[strcspn(client->name, "\n")] = '\0'; // Remove newline

    // Notify all about the new client
    char joinMessage[BUFFER_SIZE];
    sprintf(joinMessage, "%d %s 1\n", client->assignedNumber, client->name);
    printf("%s", joinMessage); // Print status on server console
    //broadcastStatus(joinMessage);
    ClientList(client->socket);

    // Main chat loop
    while (1) {
        int bytesRead = recv(client->socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break; // Client disconnected
        }

        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        int recipientIndex = atoi(buffer) - 1;

        // Validate recipient choice
        pthread_mutex_lock(&clientListLock);
        if (recipientIndex < 0 || recipientIndex >= clientCount) {
            pthread_mutex_unlock(&clientListLock);
            (client->socket, "Invalid selection.\n", 19, 0);
            continue; // Ask again
        }
        Client *recipient = clientList[recipientIndex];
        pthread_mutex_unlock(&clientListLock);

        // Chat loop for selected recipient
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            bytesRead = recv(client->socket, buffer, BUFFER_SIZE, 0);

            if (bytesRead <= 0) {
                break; // Client disconnected
            }

            buffer[strcspn(buffer, "\n")] = 0; // Remove newline
            if (strcmp(buffer, "exit") == 0) {
                (client->socket, "Exiting chat.\n", 15, 0);
                return NULL; // Exit chat loop
            }

            //  message to the selected recipient
            char message[BUFFER_SIZE];
            sprintf(message, "%s: %s\n", client->name, buffer);
            (recipient->socket, message, strlen(message), 0);

            // Ask the client to choose a recipient again after ing one message
            break;
        }
    }

    // Handle client disconnect
    close(client->socket);
    pthread_mutex_lock(&clientListLock);
    for (int i = 0; i < clientCount; i++) {
        if (clientList[i] == client) {
            for (int j = i; j < clientCount - 1; j++) {
                clientList[j] = clientList[j + 1];
            }
            clientCount--;
            break;
        }
    }
    pthread_mutex_unlock(&clientListLock);

    // Notify about disconnection
    sprintf(joinMessage, "%d %s 0\n", client->assignedNumber, client->name);
    printf("%s", joinMessage); // Print on server console
    //broadcastStatus(joinMessage);

    free(client);
    return NULL;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(clientAddr);

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    close(serverSocket);

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr;

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(serverSocket);
        exit(1);
    }

    // Main loop to accept clients
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket == -1) {
            perror("Accept failed");
            continue;
        }

        if (clientCount >= MAX_CLIENTS) {
            printf("Max clients reached. Connection refused.\n");
            close(clientSocket);
            continue;
        }

        // Create a new client handler
        Client *newClient = (Client *)malloc(sizeof(Client));
        newClient->socket = clientSocket;
        newClient->assignedNumber = clientCount + 1;  // Assign a sequential number to the client

        pthread_mutex_lock(&clientListLock);
        clientList[clientCount++] = newClient;
        pthread_mutex_unlock(&clientListLock);

        pthread_t thread;
        pthread_create(&thread, NULL, clientHandler, (void *)newClient);
        pthread_detach(thread);
    }

    close(serverSocket);
    return 0;
}
