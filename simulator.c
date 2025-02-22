#define SDL_MAIN_HANDLED
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_ttf.h"
#include <stdbool.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"
#define MAX_LINE_LENGTH 20
#define MAIN_FONT "D:\\project\\ttf\\DejaVuSans.ttf"  // Update to a valid Windows font path
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SCALE 1
#define ROAD_WIDTH 150
#define LANE_WIDTH 50
#define ARROW_SIZE 15
#define VEHICLE_SIZE 20
#define VEHICLE_SPEED 2

const char* VEHICLE_FILE = "vehicles.data";

// Road queues for each direction
VehicleQueue roadQueues[4];  // A, B, C, D

typedef struct{
    int currentLight;
    int nextLight;
} SharedData;

// Function declarations
bool initializeSDL(SDL_Window **window, SDL_Renderer **renderer);
void drawRoadsAndLane(SDL_Renderer *renderer, TTF_Font *font);
void displayText(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y);
void drawLightForB(SDL_Renderer* renderer, bool isRed);
void drawLightForA(SDL_Renderer* renderer, bool isRed);
void drawLightForC(SDL_Renderer* renderer, bool isRed);
void drawLightForD(SDL_Renderer* renderer, bool isRed);
void refreshLight(SDL_Renderer *renderer, SharedData* sharedData);
void drawVehicle(SDL_Renderer* renderer, Vehicle* vehicle);
void processVehicles(SharedData* sharedData);
void updateVehiclePosition(Vehicle* vehicle, int roadIndex, int lane);
void drawArrwow(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3);
DWORD WINAPI chequeQueue(LPVOID arg);
DWORD WINAPI readAndParseFile(LPVOID arg);
void initRoadQueues() {
    for(int i = 0; i < 4; i++) {
        initQueue(&roadQueues[i]);
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Check if road A needs priority
bool checkRoadAPriority() {
    return roadQueues[0].size >= 10;
}

// Check if road A priority should be removed
bool shouldRemoveRoadAPriority() {
    return roadQueues[0].size <= 5;
}

void printMessageHelper(const char* message, int count) {
    for (int i = 0; i < count; i++) printf("%s\n", message);
}

int main(int argc, char *argv[]) {
    HANDLE tQueue, tReadFile;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    if (!initializeSDL(&window, &renderer)) {
        return -1;
    }
    
    SharedData sharedData = { 0, 0 };
    initRoadQueues();
    
    TTF_Font* font = TTF_OpenFont(MAIN_FONT, 24);
    if (!font) SDL_Log("Failed to load font: %s", TTF_GetError());

    // Create threads for queue processing and file reading
    tQueue = CreateThread(NULL, 0, chequeQueue, &sharedData, 0, NULL);
    tReadFile = CreateThread(NULL, 0, readAndParseFile, &sharedData, 0, NULL);

    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw road system
        drawRoadsAndLane(renderer, font);
        
        // Draw all traffic lights
        drawLightForA(renderer, sharedData.currentLight != 0);
        drawLightForB(renderer, sharedData.currentLight != 1);
        drawLightForC(renderer, sharedData.currentLight != 2);
        drawLightForD(renderer, sharedData.currentLight != 3);

        // Handle light state changes
        if (sharedData.nextLight != sharedData.currentLight) {
            printf("Light of queue updated from %d to %d\n", 
                   sharedData.currentLight, sharedData.nextLight);
            sharedData.currentLight = sharedData.nextLight;
        }

        // Draw vehicles
        for (int i = 0; i < 4; i++) {
            Node* current = roadQueues[i].front;
            while (current != NULL) {
                drawVehicle(renderer, &current->vehicle);
                current = current->next;
            }
        }

        // Process vehicle movements
        processVehicles(&sharedData);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Cleanup
    if (tQueue) CloseHandle(tQueue);
    if (tReadFile) CloseHandle(tReadFile);
    for (int i = 0; i < 4; i++) {
        freeQueue(&roadQueues[i]);
    }
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

void drawVehicle(SDL_Renderer* renderer, Vehicle* vehicle) {
    // Set color to black for all vehicles
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black color

    // Draw vehicle as a rectangle
    SDL_Rect vehicleRect = {
        (int)vehicle->xPos - VEHICLE_SIZE/2,
        (int)vehicle->yPos - VEHICLE_SIZE/2,
        VEHICLE_SIZE,
        VEHICLE_SIZE
    };
    SDL_RenderFillRect(renderer, &vehicleRect);
}

void updateVehiclePosition(Vehicle* vehicle, int roadIndex, int lane) {
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int laneOffset = (lane - 2) * LANE_WIDTH;

    switch(roadIndex) {
        case 0: // Road A (North)
            vehicle->xPos = centerX + laneOffset;
            vehicle->yPos = LANE_WIDTH;
            break;
        case 1: // Road B (East)
            vehicle->xPos = WINDOW_WIDTH - LANE_WIDTH;
            vehicle->yPos = centerY + laneOffset;
            break;
        case 2: // Road C (South)
            vehicle->xPos = centerX - laneOffset;
            vehicle->yPos = WINDOW_HEIGHT - LANE_WIDTH;
            break;
        case 3: // Road D (West)
            vehicle->xPos = LANE_WIDTH;
            vehicle->yPos = centerY - laneOffset;
            break;
    }
}

bool initializeSDL(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Junction Diagram",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               WINDOW_WIDTH*SCALE, WINDOW_HEIGHT*SCALE,
                               SDL_WINDOW_SHOWN);
    if (!*window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(*renderer, SCALE, SCALE);

    if (!*renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}


void drawArrwow(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3) {
    if (y1 > y2) { swap(&y1, &y2); swap(&x1, &x2); }
    if (y1 > y3) { swap(&y1, &y3); swap(&x1, &x3); }
    if (y2 > y3) { swap(&y2, &y3); swap(&x2, &x3); }

    float dx1 = (y2 - y1) ? (float)(x2 - x1) / (y2 - y1) : 0;
    float dx2 = (y3 - y1) ? (float)(x3 - x1) / (y3 - y1) : 0;
    float dx3 = (y3 - y2) ? (float)(x3 - x2) / (y3 - y2) : 0;

    float sx1 = x1, sx2 = x1;

    for (int y = y1; y < y2; y++) {
        SDL_RenderDrawLine(renderer, (int)sx1, y, (int)sx2, y);
        sx1 += dx1;
        sx2 += dx2;
    }

    sx1 = x2;

    for (int y = y2; y <= y3; y++) {
        SDL_RenderDrawLine(renderer, (int)sx1, y, (int)sx2, y);
        sx1 += dx3;
        sx2 += dx2;
    }
}

void drawLightForC(SDL_Renderer* renderer, bool isRed){
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_Rect lightBox = {375, 300, 50, 30};
    SDL_RenderFillRect(renderer, &lightBox);
    if(isRed) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    else SDL_SetRenderDrawColor(renderer, 11, 156, 50, 255);    
    SDL_Rect straight_Light = {380, 305, 20, 20};
    SDL_RenderFillRect(renderer, &straight_Light);
    drawArrwow(renderer, 410,305, 410, 305+20, 410+10, 305+10);
}

void drawLightForA(SDL_Renderer* renderer, bool isRed){
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_Rect lightBox = {375, 450, 50, 30};  
    SDL_RenderFillRect(renderer, &lightBox);

    if(isRed) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    else SDL_SetRenderDrawColor(renderer, 11, 156, 50, 255);    
    SDL_Rect straight_Light = {400, 455, 20, 20};
    SDL_RenderFillRect(renderer, &straight_Light);

    drawArrwow(renderer, 380+10, 455, 380+10, 455+20, 380, 455+10);  // Adjust arrow direction for road A
}

void drawLightForB(SDL_Renderer* renderer, bool isRed){
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_Rect lightBox = {320, 375, 30, 50};  // Adjust position for road D
    SDL_RenderFillRect(renderer, &lightBox);

    if(isRed) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    else SDL_SetRenderDrawColor(renderer, 11, 156, 50, 255);    
    SDL_Rect straight_Light = {325, 400, 20, 20};
    SDL_RenderFillRect(renderer, &straight_Light);

    drawArrwow(renderer, 325, 380+10, 325+20, 380+10, 325+10, 380);  // Adjust arrow direction for road D
}

void drawLightForD(SDL_Renderer* renderer, bool isRed){
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_Rect lightBox = {450, 375, 30, 50};  // Adjust position for road C
    SDL_RenderFillRect(renderer, &lightBox);

    if(isRed) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    else SDL_SetRenderDrawColor(renderer, 11, 156, 50, 255);    
    SDL_Rect straight_Light = {455, 380, 20, 20};
    SDL_RenderFillRect(renderer, &straight_Light);

    drawArrwow(renderer, 455, 405, 455+20, 405, 455+10, 405+10);  // Adjust arrow direction for road C
}



void drawRoadsAndLane(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 211,211,211,255);
    SDL_Rect verticalRoad = {WINDOW_WIDTH / 2 - ROAD_WIDTH / 2, 0, ROAD_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &verticalRoad);

    SDL_Rect horizontalRoad = {0, WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2, WINDOW_WIDTH, ROAD_WIDTH};
    SDL_RenderFillRect(renderer, &horizontalRoad);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for(int i = 0; i <= 3; i++){
        SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + LANE_WIDTH*i, WINDOW_WIDTH/2 - ROAD_WIDTH/2, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + LANE_WIDTH*i);
        SDL_RenderDrawLine(renderer, 800, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + LANE_WIDTH*i, WINDOW_WIDTH/2 + ROAD_WIDTH/2, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + LANE_WIDTH*i);
        SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 - ROAD_WIDTH/2 + LANE_WIDTH*i, 0, WINDOW_WIDTH/2 - ROAD_WIDTH/2 + LANE_WIDTH*i, WINDOW_HEIGHT/2 - ROAD_WIDTH/2);
        SDL_RenderDrawLine(renderer, WINDOW_WIDTH/2 - ROAD_WIDTH/2 + LANE_WIDTH*i, 800, WINDOW_WIDTH/2 - ROAD_WIDTH/2 + LANE_WIDTH*i, WINDOW_HEIGHT/2 + ROAD_WIDTH/2);
    }
    displayText(renderer, font, "A",400, 10);
    displayText(renderer, font, "C",400,770);
    displayText(renderer, font, "D",10,400);
    displayText(renderer, font, "B",770,400);
}

void displayText(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y){
    SDL_Color textColor = {0, 0, 0, 255}; 
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {x, y, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
}

void refreshLight(SDL_Renderer *renderer, SharedData* sharedData){
    if(sharedData->nextLight == sharedData->currentLight) return;

    // Set all lights to red first
    drawLightForA(renderer, true);  // A road light red
    drawLightForB(renderer, true);  // B road light red
    drawLightForC(renderer, true);  // C road light red
    drawLightForD(renderer, true);  // D road light red

    // Now set the appropriate light to green based on nextLight
    if(sharedData->nextLight == 0) {
        drawLightForA(renderer, false);  // A road light green
    }
    else if(sharedData->nextLight == 1) {
        drawLightForB(renderer, false);  // B road light green
    }
    else if(sharedData->nextLight == 2) {
        drawLightForC(renderer, false);  // C road light green
    }
    else if(sharedData->nextLight == 3) {
        drawLightForD(renderer, false);  // D road light green
    }

    // Present the updated renderer
    SDL_RenderPresent(renderer);

    printf("Light of queue updated from %d to %d\n", sharedData->currentLight,  sharedData->nextLight);
    sharedData->currentLight = sharedData->nextLight;
}


DWORD WINAPI chequeQueue(LPVOID arg) {
    SharedData* sharedData = (SharedData*)arg;
    bool isAPriority = false;
    
    while (1) {
        // Check for road A priority
        if (!isAPriority && checkRoadAPriority()) {
            isAPriority = true;
            printf("Road A priority activated\n");
        } else if (isAPriority && shouldRemoveRoadAPriority()) {
            isAPriority = false;
            printf("Road A priority deactivated\n");
        }

        if (isAPriority) {
            // Priority mode: alternate between A and other roads
            sharedData->nextLight = 0;  // A gets green
            Sleep(5000);
            
            // Choose next road based on queue sizes
            int maxSize = 0;
            int nextRoad = 1;
            for (int i = 1; i < 4; i++) {
                if (roadQueues[i].size > maxSize) {
                    maxSize = roadQueues[i].size;
                    nextRoad = i;
                }
            }
            sharedData->nextLight = nextRoad;
            Sleep(3000);
        } else {
            // Normal rotation
            for (int i = 0; i < 4; i++) {
                sharedData->nextLight = i;
                Sleep(5000);
            }
        }
    }
    return 0;
}

void processVehicles(SharedData* sharedData) {
    int greenRoad = sharedData->currentLight;
    
    // Process all roads
    for (int roadIndex = 0; roadIndex < 4; roadIndex++) {
        if (isQueueEmpty(&roadQueues[roadIndex])) continue;

        Node* current = roadQueues[roadIndex].front;
        Node* prev = NULL;

        while (current != NULL) {
            bool shouldMove = false;
            int destRoadIndex;

            // Determine if vehicle should move
            if (current->vehicle.lane == 3) {
                // Lane 3 vehicles always move
                destRoadIndex = (roadIndex + 1) % 4;
                shouldMove = true;
            } else if (roadIndex == greenRoad && current->vehicle.lane == 2) {
                // Lane 2 vehicles move when road has green light
                destRoadIndex = current->vehicle.destinationRoad[0] - 'A';
                shouldMove = true;
            }

            if (shouldMove) {
                // Dequeue from current road
                if (prev == NULL) {
                    roadQueues[roadIndex].front = current->next;
                } else {
                    prev->next = current->next;
                }
                
                if (roadQueues[roadIndex].front == NULL) {
                    roadQueues[roadIndex].rear = NULL;
                }
                
                // Free the node and update size
                free(current);
                roadQueues[roadIndex].size--;
                
                current = prev ? prev->next : roadQueues[roadIndex].front;
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
}

DWORD WINAPI readAndParseFile(LPVOID arg) {
    SharedData* sharedData = (SharedData*)arg;
    
    while(1) {
        FILE* file = fopen(VEHICLE_FILE, "r");
        if (!file) {
            perror("Error opening file");
            Sleep(2000);
            continue;
        }

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            
            Vehicle newVehicle;
            char* vehicleId = strtok(line, ":");
            char* sourceRoad = strtok(NULL, ":");
            char* destinationRoad = strtok(NULL, ":");
            
            if (vehicleId && sourceRoad && destinationRoad) {
                strncpy(newVehicle.vehicleNumber, vehicleId, 8);
                newVehicle.vehicleNumber[8] = '\0';
                strncpy(newVehicle.sourceRoad, sourceRoad, 2);
                newVehicle.sourceRoad[2] = '\0';
                strncpy(newVehicle.destinationRoad, destinationRoad, 2);
                newVehicle.destinationRoad[2] = '\0';
                
                int sourceIndex = sourceRoad[0] - 'A';
                int destIndex = destinationRoad[0] - 'A';
                
                if ((sourceIndex + 1) % 4 == destIndex) {
                    newVehicle.lane = 3;
                } else {
                    newVehicle.lane = 2;
                }
                
                updateVehiclePosition(&newVehicle, sourceIndex, newVehicle.lane);
                
                // Try to enqueue and check if successful
                if (enqueue(&roadQueues[sourceIndex], newVehicle)) {
                    printf("Added Vehicle %s from %s to %s in lane %d\n", 
                           newVehicle.vehicleNumber, newVehicle.sourceRoad, 
                           newVehicle.destinationRoad, newVehicle.lane);
                }
            }
        }
        fclose(file);
        Sleep(2000);
    }
    return 0;
}