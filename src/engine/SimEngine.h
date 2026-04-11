#ifndef SIMENGINE_H
#define SIMENGINE_H

#include "src/core/HashMap.h"
#include "src/core/MinHeap.h"
#include "src/engine/Entity.h"
#include "src/engine/Event.h"
#include "src/engine/SimClock.h"

#include <string>

// This is a simple container for all the statistics we want to show
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
    {
      return 0.0;
    }
    return totalWaitTime / totalEntitiesFinished;
  }
};

class SimEngine
{
private:
  MinHeap eventQueue;
  HashMap entityTable;
  SimClock *simulationClock;
  SimulationStats currentStats;
  bool engineIsRunning;
  bool verboseMode;

public:
  SimEngine(double startTime, double endTime, bool showDetails = false);

  ~SimEngine();

  void scheduleEvent(double eventTime, EventType type, int entityId,
                     int locationId, std::string description = "");

  void addEntity(Entity *newEntity);

  Entity *getEntity(int entityId);

  void run();

  void reset();

  SimulationStats getStats() const;

  double getCurrentTime() const;

  void printStats() const;

private:
  // this looks at an event and decides what to do with it.
  void processEvent(Event *eventToProcess);
};

#endif