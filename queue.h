#ifndef QUEUE_H
#define QUEUE_H

#define MAX_VEHICLES 100
#include <stdbool.h>

// Vehicle structure
typedef struct {
    char vehicleNumber[9];
    char sourceRoad[3];    // Source road (e.g., "A1")
    char destinationRoad[3]; // Destination road (e.g., "B1")
    int lane;              // Lane number (1, 2, or 3)
    float xPos;           // Current x position of the vehicle
    float yPos;           // Current y position of the vehicle
} Vehicle;

// Node for the queue
typedef struct Node {
    Vehicle vehicle;
    struct Node* next;
} Node;

// VehicleQueue structure
typedef struct {
    Node* front;
    Node* rear;
    int size;
} VehicleQueue;

// Function declarations
void initQueue(VehicleQueue* q);
int isQueueEmpty(VehicleQueue* q);
bool enqueue(VehicleQueue* q, Vehicle vehicle);
Vehicle dequeue(VehicleQueue* q);
void freeQueue(VehicleQueue* q);

#endif // QUEUE_H