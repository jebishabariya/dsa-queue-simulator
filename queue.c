#include "queue.h"

// Initialize an empty queue
void initQueue(VehicleQueue* q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

// Add a vehicle to the queue
void enqueue(VehicleQueue* q, int id, const char* destination) {
    Vehicle* newVehicle = (Vehicle*)malloc(sizeof(Vehicle));
    newVehicle->id = id;
    strcpy(newVehicle->destination, destination);
    newVehicle->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newVehicle;
    } else {
        q->rear->next = newVehicle;
        q->rear = newVehicle;
    }
    q->size++;
}

// Remove a vehicle from the queue
Vehicle* dequeue(VehicleQueue* q) {
    if (q->front == NULL) return NULL; // Queue is empty
    Vehicle* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    q->size--;
    return temp;
}

// Initialize the priority queue
void initPriorityQueue(PriorityQueue* pq, int size) {
    pq->lanes = (Lane*)malloc(size * sizeof(Lane));
    pq->size = size;
    for (int i = 0; i < size; i++) {
        pq->lanes[i].id = 'A' + i;
        for (int j = 0; j < 3; j++) {
            initQueue(&pq->lanes[i].roads[j].vehicles);
        }
    }
}