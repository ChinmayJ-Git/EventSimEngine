# Project Specification — EventSimEngine

## What this project is

A generic event-driven simulation engine built in C++ from scratch.
The engine is completely decoupled from any scenario.
Two simulation scenarios are built on top of the engine.
A real-time SFML graphics window visualises everything.

## The core idea

The engine maintains a priority queue of future events sorted by time.
It always extracts the event with the earliest timestamp, advances a
virtual clock to that time, processes the event, and repeats until
the queue is empty or the end time is reached.

Time does not move second by second. It jumps to the next event.
This is called discrete event simulation.

## The two scenarios

### Hospital Queue with Triage Escalation

Patients arrive at random intervals with one of three priority levels:
- CRITICAL — priority 1 — seen immediately if any doctor is free
- NORMAL   — priority 3 — waits behind critical patients
- MINOR    — priority 5 — lowest priority

Limited doctors (configurable, default 3) and beds (default 10).

Triage escalation:
When a patient arrives, the engine schedules an ESCALATION event
for that patient in the future. If the patient is still WAITING
when the escalation fires, their priority upgrades:
- MINOR (5) becomes NORMAL (3)
- NORMAL (3) becomes CRITICAL (1)
- CRITICAL (1) does not escalate further

Escalation thresholds:
- MINOR patients escalate after 60 simulation seconds of waiting
- NORMAL patients escalate after 120 simulation seconds of waiting

After escalation from MINOR to NORMAL, a second escalation is
scheduled for another 120 seconds later.

Statistics tracked:
- Average wait time
- Longest wait time
- Total escalation count
- Doctor utilisation percentage
- Beds occupied at peak

### Traffic Light with Adaptive Signal Control

A road network modelled as a graph of intersections connected by roads.
Cars arrive at intersections at random intervals.

Fixed signal behaviour:
Each intersection has a traffic light with a base green/red timer.

Adaptive behaviour:
Every 30 simulation seconds, a SIGNAL_CHECK event fires at each
intersection. The signal counts how many cars are waiting in each
direction. If one direction has more than twice the cars of the
other, it gets an extended green phase proportional to the ratio.

Statistics tracked:
- Average car wait time per intersection
- Total cars that passed through
- Busiest intersection
- Total cars still waiting at simulation end

## Engine files

Event.h
  Defines the Event struct and EventType enum.
  EventType values: ARRIVAL, DEPARTURE, SERVICE_START, SERVICE_END,
                    SIGNAL_CHANGE, SIGNAL_CHECK, ESCALATION, TICK, CUSTOM

Entity.h
  Defines the Entity struct and EntityType, EntityState enums.
  EntityType values: PATIENT, DOCTOR, BED, CAR, TRAFFIC_LIGHT, GENERIC
  EntityState values: IDLE, BUSY, WAITING, MOVING, FINISHED
  Key field: escalationDeadline (double) — -1 if not set

SimClock.h
  Manages virtual time. advance(t) jumps clock to time t.
  hasSimulationEnded() returns true when currentTime >= endTime.
  reset() brings clock back to startTime for repeat runs.

SimEngine.h / SimEngine.cpp
  The main engine class.
  Contains EventQueueStub (to be replaced by real MinHeap)
  Contains EntityTableStub (to be replaced by real HashMap)
  Key function: run() — the main simulation loop
  Key function: processEvent() — handles each event type
  Key function: scheduleEvent() — adds new event to queue
  Key function: addEntity() — registers entity in table

EscalationManager.h
  Static helper class.
  scheduleEscalation(patient, engine) — called when patient arrives
  processEscalation(patient, engine) — called when ESCALATION fires

## Data structure files (src/core/)

All built from scratch. No STL containers anywhere.

DynamicArray.h  — resizable array, replaces std::vector
MinHeap.h       — binary min-heap, used as event priority queue
                  must support: insert(Event*), extractMinimum(),
                  peekMinimum(), isEmpty()
                  must compare Events using operator<  (by time)
HashMap.h       — hash map with chaining using linked lists
                  must support: insert(int key, Entity* value),
                  get(int key), remove(int key), contains(int key)
LinkedList.h    — doubly linked list, used for hash map chaining
                  and event history logs
Graph.h         — adjacency list graph, used for road network
                  must support: addNode(int id), addEdge(int from, int to),
                  getNeighbours(int id), BFS(int start, int end)

## GUI files (src/gui/)

Built with SFML 2.6.1. SFML is at C:\SFML on Windows.
Allowed SFML modules: sfml-graphics, sfml-window, sfml-system

Window.h / Window.cpp
  Main SFML window. 1280x720 resolution.
  Handles window creation, event polling, open/close.

Renderer.h / Renderer.cpp
  Draws entities as shapes on screen.
  Hospital: patients as circles (colour by priority), doctors as rectangles
  Traffic: cars as small rectangles, intersections as larger squares,
           roads as lines between intersections
  Updates every frame based on current engine state.

Dashboard.h
  Stats panel on the right side of the screen.
  Shows live updating numbers: wait times, counts, utilisation.

Menu.h
  Opening screen. Shows two options: Hospital Simulation, Traffic Simulation.
  User clicks to choose. Passes choice to main.cpp.

## Scenario files (src/scenarios/)

HospitalSim.h / HospitalSim.cpp
  Uses SimEngine, EscalationManager.
  Configurable: number of doctors, number of beds, simulation duration,
                patient arrival rate.
  On patient arrival: adds entity, schedules escalation, assigns doctor
                      if one is free or adds to waiting queue.
  On service end: frees doctor, checks waiting queue, assigns next patient
                  by highest priority (lowest priority number).

TrafficSim.h / TrafficSim.cpp
  Uses SimEngine, Graph.
  Configurable: number of intersections, roads between them,
                car arrival rate, base signal timer, simulation duration.
  On SIGNAL_CHECK: counts waiting cars per direction, adjusts timing.
  On SIGNAL_CHANGE: moves waiting cars through, schedules next change.

## Analytics files (src/analytics/)

StatsCollector.h
  Collects and stores statistics during simulation run.
  Supports multiple runs — stores results per run for comparison.
  Uses DynamicArray internally (not std::vector).

CSVExporter.h
  Writes simulation results to a .csv file after each run.
  Uses <fstream> only. No external libraries.

## Strict rules — must be followed everywhere

1. No STL containers. No vector, map, queue, set, unordered_map, etc.
   Use DynamicArray, HashMap, LinkedList, Graph from src/core/ instead.

2. Allowed standard includes only:
   <string> <iostream> <fstream> <cmath> <cstdlib> <ctime>

3. Every .h file must have header guards:
   #ifndef FILENAME_H
   #define FILENAME_H
   #endif

4. Naming conventions:
   Classes:   CapitalFirst        — SimEngine, MinHeap
   Variables: camelCase           — patientArrivalTime
   Functions: camelCase           — scheduleEvent()
   Constants: ALL_CAPS            — MAX_QUEUE_SIZE

5. Comment style: short phrases only, 3-5 words max
   // calc wait time
   // extract earliest event
   Not full sentences.

6. Code style: simple and readable. No clever one-liners.
   Every operation on its own line.
   Functions under 40 lines where possible.

7. Time values: always double
   IDs: always int
   Names: always std::string
   Flags: always bool

## What needs to be done — priority order

1. Replace EventQueueStub in SimEngine.h with real MinHeap from src/core/MinHeap.h
2. Replace EntityTableStub in SimEngine.h with real HashMap from src/core/HashMap.h
3. Complete HospitalSim.h and HospitalSim.cpp with triage escalation
4. Complete TrafficSim.h and TrafficSim.cpp with adaptive signal control
5. Complete Renderer.cpp to draw both scenarios
6. Write final main.cpp connecting everything
7. Verify full compile with SFML

## Compile command (Windows, MinGW, SFML at C:\SFML)

g++ main.cpp src/engine/SimEngine.cpp src/gui/Window.cpp src/gui/Renderer.cpp src/scenarios/HospitalSim.cpp src/scenarios/TrafficSim.cpp -o EventSimEngine.exe -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system

## Engine test compile (no SFML needed)

g++ tests/test_engine.cpp src/engine/SimEngine.cpp -o tests/test_engine.exe -I.