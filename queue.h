#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Vehicle structure
typedef struct Vehicle {
    int id; // Unique identifier for the vehicle
    char destination[4]; // Destination road (e.g., "A1", "B2", etc.)
    struct Vehicle* next;
} Vehicle;

// Vehicle queue structure
typedef struct {
    Vehicle* front;
    Vehicle* rear;
    int size;
} VehicleQueue;

// Road structure
typedef struct {
    VehicleQueue vehicles; // Queue of vehicles waiting in this road
} Road;

// Lane structure
typedef struct {
    char id; // Lane identifier (e.g., 'A', 'B', 'C', 'D')
    Road roads[3]; // Three roads in the lane
} Lane;

// Priority queue structure
typedef struct {
    Lane* lanes; // Array of lanes
    int size; // Number of lanes (4 in this case: A, B, C, D)
} PriorityQueue;

// Function declarations
void initQueue(VehicleQueue* q);
void enqueue(VehicleQueue* q, int id, const char* destination);
Vehicle* dequeue(VehicleQueue* q);
void initPriorityQueue(PriorityQueue* pq, int size);

#endif