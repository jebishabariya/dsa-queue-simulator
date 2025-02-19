#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // Windows-specific socket library
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define PORT 5000
#define BUFFER_SIZE 100
#define VEHICLE_FILE "vehicles.data"

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Start listening
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept connections
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Client connected...\n");

    FILE *file = fopen(VEHICLE_FILE, "a");
    if (!file) {
        perror("Failed to open file");
        closesocket(new_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    while (1) {
        int bytes_read = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        buffer[bytes_read] = '\0'; // Null-terminate the received data
        fprintf(file, "%s\n", buffer); // Write to file
        fflush(file); // Ensure data is written immediately
        printf("Received: %s\n", buffer);
        memset(buffer, 0, BUFFER_SIZE); // Clear buffer
    }

    fclose(file);
    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
