# MediSim Engine

MediSim Engine is a C++ event-driven hospital simulation project.
It simulates patient arrivals, waiting queues, triage escalation, doctor assignment, live dashboard updates, and CSV result export.

## What This Project Includes

- Discrete event simulation core (arrival, treatment start, treatment end, escalation)
- Three patient categories: NORMAL, SPECIFIC, CRITICAL
- Three doctor roles: General, Specialist, Emergency
- Live SFML dashboard: waiting area, treatment cards, event log, live stats
- CSV export to results.csv after each run

## Requirements

- Windows 10/11
- MinGW-w64 with g++ (C++17)
- SFML 2.6.x installed at C:\SFML
- assets/font.ttf present in the project

## Build

Run from the project root (folder containing main.cpp):

g++ -std=c++17 -IC:/SFML/include -LC:/SFML/lib main.cpp src/engine/SimEngine.cpp src/gui/Window.cpp src/gui/Menu.cpp src/gui/Renderer.cpp src/scenarios/HospitalSim.cpp -o EventSimEngine.exe -lsfml-graphics -lsfml-window -lsfml-system

## Run

PowerShell:

$env:Path = "C:\SFML\bin;$env:Path"
.\EventSimEngine.exe

## Output

- Simulation results are written to results.csv
- Each run appends one row with runtime, waits, escalations, utilization, and waiting count

## Repository

https://github.com/ChinmayJ-Git/EventSimEngine
