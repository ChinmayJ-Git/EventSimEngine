#ifndef SIMENGINE_H
#define SIMENGINE_H

#include "../core/HashMap.h"
#include "../core/MinHeap.h"
#include "Entity.h"
#include "Event.h"
#include "SimClock.h"

#include <string>

// simulation statistics
struct SimulationStats
{
    int totalEntitiesCreated;
    int totalEntitiesFinished;
    int totalEventsProcessed;
    double totalWaitTime;
    double longestWaitTime;
    double shortestWaitTime;
    std::string scenarioName;

    SimulationStats()
    {
        totalEntitiesCreated = 0;
        totalEntitiesFinished = 0;
        totalEventsProcessed = 0;
        totalWaitTime = 0.0;
        longestWaitTime = 0.0;
        shortestWaitTime = 999999.0;
        scenarioName = "Unknown";
    }

    double getAverageWaitTime() const
    {
        if (totalEntitiesFinished == 0)
            return 0.0;
        return totalWaitTime / totalEntitiesFinished;
    }
};

// callback interface — scenarios implement this
class EventHandler
{
public:
    virtual void onEvent(Event* e) = 0;
    virtual ~EventHandler() {}
};

class SimEngine
{
private:
    MinHeap eventQueue;
    HashMap entityTable;
    SimClock* simulationClock;
    SimulationStats currentStats;
    bool engineIsRunning;
    bool verboseMode;

    // scenario callback — called for every event
    EventHandler* scenarioHandler;

public:
    SimEngine(double startTime, double endTime, bool showDetails = false);
    ~SimEngine();

    // register scenario handler
    void setEventHandler(EventHandler* handler);

    void scheduleEvent(double eventTime, EventType type, int entityId,
                       int locationId, std::string description = "");

    void addEntity(Entity* newEntity);
    Entity* getEntity(int entityId);

    void run();
    bool processOneEvent();
    void reset();

    SimulationStats getStats() const;
    double getCurrentTime() const;
    void printStats() const;

private:
    void processEvent(Event* eventToProcess);
};

#endif