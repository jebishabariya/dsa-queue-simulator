#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

// Initialize an empty queue
void initQueue(VehicleQueue* q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

// Check if the queue is empty
int isQueueEmpty(VehicleQueue* q) {
    return (q->size == 0);
}

// Add a vehicle to the queue
void enqueue(VehicleQueue* q, Vehicle vehicle) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Queue allocation failed");
        exit(1);
    }
    newNode->vehicle = vehicle;
    newNode->next = NULL;

    if (q->rear != NULL) {
        q->rear->next = newNode;
    }
    q->rear = newNode;
    if (q->front == NULL) {
        q->front = newNode;
    }
    q->size++;
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
