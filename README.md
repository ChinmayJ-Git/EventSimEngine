# EventSimEngine

A generic event-driven simulation engine written in C++.

## Overview

The engine maintains a queue of future events sorted by time, processes them in chronological order, and updates system state accordingly. It is completely decoupled from any specific scenario. Three simulations are built on top of it:

- **Hospital Queue** — patients with priority levels, limited doctors and beds, wait time tracking
- **Network Packet Routing** — packets traversing a router graph, queue limits, drop rate analysis  
- **Traffic Lights** — vehicles queuing at signal-controlled intersections, congestion measurement

Time does not move one second at a time. The clock jumps directly to the next event. This is discrete event simulation, fast enough to process hundreds of thousands of events in seconds.

## Data Structures
| Min-Heap | Event queue — O(log n) insert and extract-min |
| Hash Map | Entity lookup by ID — O(1) average with chaining |
| Graph | Adjacency list for router and road networks, BFS for shortest path |
| Linked List | Event history log and router packet buffers |
| Dynamic Array | General resizable storage |

## Engine Design
while event queue is not empty AND time has not ended:
extract earliest event from min-heap
advance clock to that event's time
process event — update entities, schedule new events
collect statistics

Scenarios register entities and schedule their first events. The engine handles everything after that.

## Project Structure
src/
├── core/        — data structures
├── engine/      — SimEngine, Event, Entity, SimClock
├── gui/         — SFML window, renderer, dashboard
├── scenarios/   — HospitalSim, NetworkSim, TrafficSim
└── analytics/   — StatsCollector, CSVExporter

## Build

Requires MinGW-w64 and SFML 2.6.1 at `C:\SFML`.
```bash
g++ main.cpp src/engine/SimEngine.cpp src/gui/Window.cpp src/gui/Renderer.cpp src/scenarios/HospitalSim.cpp src/scenarios/NetworkSim.cpp src/scenarios/TrafficSim.cpp -o EventSimEngine.exe -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```
Engine test without SFML:
```bash
g++ tests/test_engine.cpp -o tests/test_engine.exe -I.
tests/test_engine.exe
```