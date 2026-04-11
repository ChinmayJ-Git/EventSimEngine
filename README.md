# EventSimEngine

A generic event-driven simulation engine written in C++.

---

## Overview

The engine maintains a queue of future events sorted by time, processes them in chronological order, and updates system state accordingly. It is completely decoupled from any specific scenario. Two simulations are built on top of it:

- **Hospital Queue with Triage Escalation** — patients arrive with priority levels, limited doctors and beds, and if a patient waits too long without being seen their priority automatically upgrades. A minor patient becomes normal, a normal patient becomes critical. This is triggered by an ESCALATION event scheduled by the engine the moment a patient arrives.

- **Traffic Lights with Adaptive Signal Control** — vehicles queue at signal-controlled intersections across a road network. Instead of fixed timers, signals periodically check how many cars are waiting in each direction and adjust green time proportionally. The busier direction always gets more time.

---

## What makes our project different

Most student simulations are hardcoded for one use case. This engine is not. The engine does not know what a patient or a traffic light is. It just knows that at t=9.05 something happens and at t=9.22 something else happens, and it processes them in that order. The scenarios plug in on top.

The two innovative features — triage escalation and adaptive signals. Triage escalation dynamically reorders the waiting queue mid-simulation as patients upgrade priority. Adaptive signal control means no two simulation runs produce identical signal timings because the signals react to what is actually happening.

---
```
## Project Structure
EventSimEngine/
├── src/
│   ├── core/              — data structures
│   │   ├── DynamicArray.h
│   │   ├── MinHeap.h
│   │   ├── HashMap.h
│   │   ├── LinkedList.h
│   │   └── Graph.h
│   ├── engine/            — simulation core
│   │   ├── Event.h
│   │   ├── Entity.h
│   │   ├── SimClock.h
│   │   ├── SimEngine.h
│   │   ├── SimEngine.cpp
│   │   └── EscalationManager.h
│   ├── gui/               — SFML graphics
│   │   ├── Window.h / Window.cpp
│   │   ├── Renderer.h / Renderer.cpp
│   │   ├── Dashboard.h
│   │   └── Menu.h
│   ├── scenarios/         — simulation worlds
│   │   ├── HospitalSim.h / HospitalSim.cpp
│   │   └── TrafficSim.h / TrafficSim.cpp
│   └── analytics/         — output and statistics
│       ├── StatsCollector.h
│       └── CSVExporter.h
├── tests/
├── docs/
├── assets/
└── main.cpp
```
---

## How to Build

Requires MinGW-w64 and SFML 2.6.1 at `C:\SFML`.

```bash
g++ main.cpp src/engine/SimEngine.cpp src/gui/Window.cpp src/gui/Renderer.cpp src/scenarios/HospitalSim.cpp src/scenarios/TrafficSim.cpp -o EventSimEngine.exe -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```

Engine test without SFML:

```bash
g++ tests/test_engine.cpp src/engine/SimEngine.cpp -o tests/test_engine.exe -I.
tests/test_engine.exe
```

*C++ — SFML 2.6.1*