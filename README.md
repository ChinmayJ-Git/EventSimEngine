# EventSimEngine

A Hospital Simulator built in C++ for a Data Structures and Algorithms course project.

## What It Is

This project is a discrete event simulation engine with a hospital scenario built on top of it.
Instead of updating time second-by-second, the engine jumps directly to the next event in the queue.
That makes the simulation efficient, deterministic, and ideal for modeling hospital flow.

At runtime, the system schedules arrivals, service starts, service ends, escalations, and departures,
then processes them in chronological order.

## Features

- Discrete event simulation architecture with a reusable C++ engine
- Three patient types: Critical, Urgent, Normal
- Three specialized doctors: Emergency, General, Specialist
- Triage escalation for long-waiting patients
- SFML-based visualization of the simulation state
- CSV export for simulation results and post-run analysis

## DSA Used

- MinHeap: event queue ordered by event timestamp
- HashMap: entity storage and lookup by ID
- LinkedList: waiting queues and chained storage logic
- DynamicArray: dynamic stats storage and list operations

## Build Command

Requires MinGW-w64 and SFML 2.6.1 installed at `C:\SFML`.
The UI requires `assets/font.ttf` to be present.

```bash
g++ main.cpp src/engine/SimEngine.cpp src/gui/Window.cpp src/gui/Menu.cpp src/gui/Renderer.cpp src/scenarios/HospitalSim.cpp -o EventSimEngine.exe -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```

## Project Structure

```text
EventSimEngine/
├── main.cpp
├── PROJECT_SPEC.md
├── README.md
├── assets/
├── docs/
├── src/
│   ├── analytics/
│   │   ├── CSVExporter.h
│   │   └── StatsCollector.h
│   ├── core/
│   │   ├── DynamicArray.h
│   │   ├── HashMap.h
│   │   ├── LinkedList.h
│   │   └── MinHeap.h
│   ├── engine/
│   │   ├── Entity.h
│   │   ├── EscalationManager.h
│   │   ├── Event.h
│   │   ├── SimClock.h
│   │   ├── SimEngine.cpp
│   │   └── SimEngine.h
│   ├── gui/
│   │   ├── Dashboard.h
│   │   ├── Menu.h
│   │   ├── Renderer.cpp
│   │   ├── Renderer.h
│   │   ├── Window.cpp
│   │   └── Window.h
│   └── scenarios/
│       ├── HospitalSim.cpp
│       └── HospitalSim.h
└── tests/
    └── test_engine.cpp
```

This project demonstrates practical DSA design in a complete simulation workflow,
from core event scheduling to visual output and analytics export.