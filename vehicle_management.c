#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "vehicle_management.h"

#define MAX_VEHICLE_ID_LEN 10
#define MAX_QUEUE_SIZE 100

// Vehicle struct to store vehicle info
typedef struct {
    char id[MAX_VEHICLE_ID_LEN];
    char currentLane;
    char destination;
} Vehicle;

// Queue for each lane (A, B, C, D)
typedef struct {
    Vehicle queue[MAX_QUEUE_SIZE];
    int front;
    int rear;
} LaneQueue;

// Initialize the lane queue
void initQueue(LaneQueue* laneQueue) {
    laneQueue->front = 0;
    laneQueue->rear = 0;
}

// Check if the lane queue is full
int isQueueFull(LaneQueue* laneQueue) {
    return laneQueue->rear == MAX_QUEUE_SIZE;
}

// Check if the lane queue is empty
int isQueueEmpty(LaneQueue* laneQueue) {
    return laneQueue->front == laneQueue->rear;
}

// Add vehicle to lane queue
void enqueue(LaneQueue* laneQueue, Vehicle vehicle) {
    if (isQueueFull(laneQueue)) {
        printf("Queue is full!\n");
        return;
    }
    laneQueue->queue[laneQueue->rear] = vehicle;
    laneQueue->rear++;
}

// Remove vehicle from lane queue
Vehicle dequeue(LaneQueue* laneQueue) {
    if (isQueueEmpty(laneQueue)) {
        printf("Queue is empty!\n");
        return (Vehicle){"", ' ', ' '}; // Empty vehicle
    }
    Vehicle vehicle = laneQueue->queue[laneQueue->front];
    laneQueue->front++;
    return vehicle;
}

// Function to write a vehicle to a file corresponding to its destination
void writeVehicleToFile(Vehicle vehicle) {
    char filename[20];
    sprintf(filename, "lane%c.txt", vehicle.destination);

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    fprintf(file, "%s,%c,%c\n", vehicle.id, vehicle.currentLane, vehicle.destination);
    fclose(file);
}

// Function to randomly choose a file that is not the vehicle's destination
void assignToRandomFile(Vehicle* vehicle) {
    char lanes[] = {'A', 'B', 'C', 'D'};
    char availableLanes[3];
    int count = 0;

    // Prepare available lanes (exclude the destination lane)
    for (int i = 0; i < 4; i++) {
        if (lanes[i] != vehicle->destination) {
            availableLanes[count++] = lanes[i];
        }
    }

    // Randomly choose one of the available lanes
    int randomIndex = rand() % 3;
    vehicle->destination = availableLanes[randomIndex];

    // Write the vehicle to the randomly selected lane file
    writeVehicleToFile(*vehicle);
}

// Function to load vehicles from the vehicles.data file
void loadVehiclesFromDataFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening vehicles data file\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        Vehicle vehicle;
        sscanf(line, "%[^,],%c,%c", vehicle.id, &vehicle.currentLane, &vehicle.destination);

        // Assign vehicle to a random file (excluding its destination file)
        assignToRandomFile(&vehicle);
    }

    fclose(file);
}

int main() {
    srand(time(NULL)); // Seed for random number generation

    // Load vehicles from vehicles.data file
    loadVehiclesFromDataFile("vehicles.data");

    return 0;
}
