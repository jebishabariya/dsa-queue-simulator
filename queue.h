#ifndef QUEUE_H
#define QUEUE_H

#define MAX_VEHICLES 100
#include <stdbool.h>
#include <stdint.h>

// Vehicle structure
typedef struct {
    float xPos;
    float yPos;
    float targetX;
    float targetY;
    bool isMoving;
    char vehicleNumber[9];
    char sourceRoad[3];
    char destinationRoad[3];
    int lane;
    uint32_t lastMoveTime;
    uint32_t enqueueTime; 
    //int enqueueTime;
    bool isActive;
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
bool enqueue(VehicleQueue* q, Vehicle* vehicle);
Vehicle dequeue(VehicleQueue* q);
void freeQueue(VehicleQueue* q);
void clearQueue(VehicleQueue* queue);

#endif // QUEUE_H