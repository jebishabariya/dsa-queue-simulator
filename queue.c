#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_ttf.h"

// Initialize an empty queue
void initQueue(VehicleQueue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// Check if the queue is empty
int isQueueEmpty(VehicleQueue* q) {
    return (q->size == 0);
}

// Add a vehicle to the queue
bool enqueue(VehicleQueue* queue, Vehicle* vehicle) {
    // Check if queue has reached maximum size
    if (queue->size >= 15) {
        printf("Queue is full (max 15 vehicles). Vehicle rejected.\n");
        return false;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return false;
    }
    
    newNode->vehicle = *vehicle;  // Copy vehicle data
    newNode->vehicle.isActive = true;  // Mark as active
    newNode->vehicle.enqueueTime = SDL_GetTicks(); 
    newNode->next = NULL;

    
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    
    queue->size++;
    return true;
}

// Remove a vehicle from the queue
Vehicle dequeue(VehicleQueue* q) {
    if (isQueueEmpty(q)) {
        perror("Queue is empty");
        exit(1);
    }

    Node* temp = q->front;
    Vehicle vehicle = temp->vehicle;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->size--;
    return vehicle;
}

// Free the entire queue
void freeQueue(VehicleQueue* q) {
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }
}
void clearQueue(VehicleQueue* queue) {
    while (queue->front != NULL) {
        Node* temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    queue->rear = NULL;
}