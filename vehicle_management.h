#ifndef VEHICLE_MANAGEMENT_H
#define VEHICLE_MANAGEMENT_H

typedef struct {
    char id[10];
    char currentLane;
    char destination;
} Vehicle;

void loadVehiclesFromDataFile(const char *filename);
void assignToRandomFile(Vehicle* vehicle);
void writeVehicleToFile(Vehicle vehicle);

#endif // VEHICLE_MANAGEMENT_H
