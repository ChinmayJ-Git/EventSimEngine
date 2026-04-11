#ifndef SIMENGINE_H
#define SIMENGINE_H

#include "Event.h"
#include "Entity.h"
#include "SimClock.h"

#include <string>

struct EventQueueStub
{
  static const int MAXIMUM_EVENTS = 10000;
  Event *storedEvents[MAXIMUM_EVENTS];
  int numberOfEvents;

  EventQueueStub()
  {
    numberOfEvents = 0;
  }

  bool isEmpty() const
  {
    return numberOfEvents == 0;
  }

  void insert(Event *newEvent)
  {
    if (numberOfEvents < MAXIMUM_EVENTS)
    {
      storedEvents[numberOfEvents] = newEvent;
      numberOfEvents++;
    }
  }

  Event *extractMinimum()
  {
    if (isEmpty())
      return nullptr;

    int indexOfMinimum = 0;
    for (int i = 1; i < numberOfEvents; i++)
    {
      if (storedEvents[i]->time < storedEvents[indexOfMinimum]->time)
      {
        indexOfMinimum = i;
      }
    }

    Event *minimumEvent = storedEvents[indexOfMinimum];

    storedEvents[indexOfMinimum] = storedEvents[numberOfEvents - 1];

    numberOfEvents--;

    return minimumEvent;
  }

  Event *peekMinimum() const
  {
    if (isEmpty())
      return nullptr;
    int indexOfMinimum = 0;
    for (int i = 1; i < numberOfEvents; i++)
    {
      if (storedEvents[i]->time < storedEvents[indexOfMinimum]->time)
      {
        indexOfMinimum = i;
      }
    }
    return storedEvents[indexOfMinimum];
  }
};

struct EntityTableStub
{
  static const int MAXIMUM_ENTITIES = 1000;
  Entity *storedEntities[MAXIMUM_ENTITIES];
  int numberOfEntities;

  EntityTableStub()
  {
    numberOfEntities = 0;
  }

  void addEntity(Entity *newEntity)
  {
    if (numberOfEntities < MAXIMUM_ENTITIES)
    {
      storedEntities[numberOfEntities] = newEntity;
      numberOfEntities++;
    }
  }

  Entity *findEntityById(int searchId)
  {
    for (int i = 0; i < numberOfEntities; i++)
    {
      if (storedEntities[i]->id == searchId)
      {
        return storedEntities[i];
      }
    }

    return nullptr;
  }

  int getCount() const
  {
    return numberOfEntities;
  }
};

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
  EventQueueStub eventQueue;
  EntityTableStub entityTable;
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