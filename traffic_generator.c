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
char* generateLane() {
    char* lanes[] = {"A1", "B1", "C1", "D1"};
    return lanes[rand() % 4];
}

const char* getSourceRoad(const char* destinationRoad) {
    if (strcmp(destinationRoad, "A1") == 0) 
    {
        char* lanes[]={"D3","B2","C2"};
        return lanes[rand()%3];
    };
    if (strcmp(destinationRoad, "B1") == 0)  {
        char* lanes[]={"A3","D2","C2"};
        return lanes[rand()%3];
    };
    if (strcmp(destinationRoad, "C1") == 0)  {
        char* lanes[]={"B3","A2","D2"};
        return lanes[rand()%3];
    };
    if (strcmp(destinationRoad, "D1") == 0)  {
        char* lanes[]={"C3","B2","A2"};
        return lanes[rand()%3];
    };
    // Default case (should not happen)
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
        char* lane = generateLane();
        

        char destinationRoad[3];
        snprintf(destinationRoad, sizeof(destinationRoad), "%s", lane);

        // Determine source road based on destination road
        const char* sourceRoad = getSourceRoad(destinationRoad);

        snprintf(buffer, BUFFER_SIZE, "%s:%s:%s", vehicle, sourceRoad, destinationRoad);

        // Send message
        send(sock, buffer, strlen(buffer), 0);
        printf("Sent: %s\n", buffer);

        Sleep(5000); // Windows equivalent of sleep(1)
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    
    return 0;
}
