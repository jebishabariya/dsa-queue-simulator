#ifndef QUEUE_H
#define QUEUE_H

#define MAX_VEHICLES 100

// Vehicle structure
typedef struct {
    char vehicleNumber[9];
    char lane; // A, B, C, D (East, North, West, South)
    int xPos;  // x position for the vehicle
    int yPos;  // y position for the vehicle
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
void enqueue(VehicleQueue* q, Vehicle vehicle);
Vehicle dequeue(VehicleQueue* q);
void freeQueue(VehicleQueue* q);

#endif // QUEUE_H
