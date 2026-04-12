#ifndef TRAFFICSIM_H
#define TRAFFICSIM_H

#include "../core/DynamicArray.h"
#include "../engine/SimEngine.h"

#include <string>

class TrafficSim
{
private:
    SimEngine *engine;
    int numberOfIntersections;
    double duration;
    double carArrivalGap;

    DynamicArray<int> lightStates;
    DynamicArray<double> signalTimers;
    DynamicArray<int> lightEntityIds;
    DynamicArray<DynamicArray<int> *> waitingCarIds;
    DynamicArray<DynamicArray<double> *> waitingArrivalTimes;
    DynamicArray<int> carsPassedPerIntersection;

    int nextEntityId;

    int totalCars;
    int totalPassed;
    double totalCarWaitTime;
    double longestCarWait;
    int busiestIntersection;

public:
    TrafficSim(SimEngine *engine, int numberOfIntersections,
               double duration, double carArrivalGap);
    ~TrafficSim();

    void initialise();
    void run();
    void printResults();

private:
    void buildRoadGraph();
    void handleArrival(Event *eventData);
    void handleSignalCheck(Event *eventData);
    void handleSignalChange(Event *eventData);
    void handleDeparture(Event *eventData);
    void updateBusiestByQueue(int intersectionId);
    double getRandomCarGap() const;
};

#endif