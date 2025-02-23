# dsa-queue-simulator
1st assigment of 2nd Year 1st Sem

Traffic Simulation Project
This project simulates traffic flow at a four-way intersection with traffic lights and multiple lanes. The simulation demonstrates vehicle movement, lane changing, and traffic light management for optimal traffic flow.

## Features
Dynamic Vehicle Handling: Vehicles are enqueued into their respective lanes upon arrival.

Lane-Specific Logic:

Lane 2: Vehicles move forward with a controlled gap and stop at the junction if the light is red. If green, they proceed to their destination.

Lane 1: Vehicles appear at the front of the road and are dequeued when they reach at the end of lane.

Lane 3: Vehicles move to the junction and directly enter their destination without checking the light.

Priority Lane:

A2 has priority: If A2 has more than 10 vehicles, all other lanes receive red signals until A2's count drops below 5.

File Input & Socket Integration: Vehicle data is read from a file or received via a socket.

Real-Time Updates: The system continuously processes incoming vehicles and updates their positions.

## System Design
Each road has three lanes:

Lane 1: Incoming vehicles (front-queued, dequeued periodically).

Lane 2: Vehicles moving towards the junction (traffic-light dependent).

Lane 3: Outgoing vehicles (light-independent).

Junction Control: Traffic lights dictate movement in Lane 2, while Lane 1 & 3 operate independently.

Queue Management:

Each lane is represented as a queue.

Vehicles move forward and dequeue upon reaching the junction.
## Prerequisites:
Before running this project, make sure you have the following installed:

-SDL2 Library
-C Compiler (GCC recommended)

To Build :
First complie the receiver2.c: gcc receiver2.c -o receiver2.exe -lws2_32  
Then compile traffic_generator.c: gcc -o traffic_generator.exe traffic_generator.c -lws2_32
Run both of them and compile simulator.c: gcc simulator.c queue.c -o simulator -lSDL2 -lSDL2_ttf

Run the simulation.

## DEMO
![Traffic Simulation Demo](./trafficsimulation.gif)

## References:
SDL2 Documentation: https://wiki.libsdl.org/SDL2/FrontPage
SDL2_ttf Documentation:https://github.com/libsdl-org/SDL_ttf

