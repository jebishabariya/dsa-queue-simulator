#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // Windows socket library
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define SERVER_IP "127.0.0.1" // Change if needed
#define PORT 5000
#define BUFFER_SIZE 100

// Generate a random vehicle number
void generateVehicleNumber(char* buffer) {
    buffer[0] = 'A' + rand() % 26;
    buffer[1] = 'A' + rand() % 26;
    buffer[2] = '0' + rand() % 10;
    buffer[3] = 'A' + rand() % 26;
    buffer[4] = 'A' + rand() % 26;
    buffer[5] = '0' + rand() % 10;
    buffer[6] = '0' + rand() % 10;
    buffer[7] = '0' + rand() % 10;
    buffer[8] = '\0';
}

// Generate a random lane
char generateLane() {
    char lanes[] = {'A', 'B', 'C', 'D'};
    return lanes[rand() % 4];
}
int generateLanenum(){
    char num[]={1,2,3};
    return num[rand()%3];
}
int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        printf("Invalid address.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to server...\n");

    srand(time(NULL));

    while (1) {
        char vehicle[9];
        generateVehicleNumber(vehicle);
        char lane = generateLane();
        int num = generateLanenum();

        snprintf(buffer, BUFFER_SIZE, "%s:%c%d", vehicle, lane,num);

        // Send message
        send(sock, buffer, strlen(buffer), 0);
        printf("Sent: %s\n", buffer);

        Sleep(1000); // Windows equivalent of sleep(1)
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    
    return 0;
}
