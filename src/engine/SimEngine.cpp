#include "SimEngine.h"
#include <iostream>

// set up engine
SimEngine::SimEngine(double startTime, double endTime, bool showDetails)
{
  simulationClock = new SimClock(startTime, endTime);
  engineIsRunning = false;
  verboseMode = showDetails;
  currentStats = SimulationStats();
}

// clean up memory
SimEngine::~SimEngine()
{
  delete simulationClock;
}

// add event to queue
void SimEngine::scheduleEvent(double eventTime, EventType type, int entityId,
                              int locationId, std::string description)
{
  Event *newEvent = new Event(eventTime, type, entityId, locationId, description);
  eventQueue.insert(newEvent);
}

// register entity
void SimEngine::addEntity(Entity *newEntity)
{
  entityTable.insert(newEntity->id, newEntity);
  currentStats.totalEntitiesCreated++;
}

// find entity by id
Entity *SimEngine::getEntity(int entityId)
{
  return entityTable.get(entityId);
}

// current sim time
double SimEngine::getCurrentTime() const
{
  return simulationClock->getCurrentTime();
}

// return stats
SimulationStats SimEngine::getStats() const
{
  return currentStats;
}

// main simulation loop
void SimEngine::run()
{
  engineIsRunning = true;

  std::cout << "Simulation started." << std::endl;
  std::cout << "Running until time: " << simulationClock->getEndTime() << std::endl;

  while (!eventQueue.isEmpty() && !simulationClock->hasSimulationEnded())
  {
    Event *nextEvent = eventQueue.extractMinimum();

    if (nextEvent == nullptr)
      break;

    simulationClock->advance(nextEvent->time);

    if (simulationClock->hasSimulationEnded())
    {
      delete nextEvent;
      break;
    }

    if (verboseMode)
    {
      std::cout << "Time " << nextEvent->time
                << " | " << nextEvent->description
                << " | Entity: " << nextEvent->entityId
                << std::endl;
    }

    processEvent(nextEvent);
    currentStats.totalEventsProcessed++;
    delete nextEvent;
  }

  engineIsRunning = false;
  std::cout << "Simulation complete." << std::endl;
  std::cout << "Total events processed: " << currentStats.totalEventsProcessed << std::endl;
}

// handle one event
void SimEngine::processEvent(Event *eventToProcess)
{
  Entity *involvedEntity = entityTable.get(eventToProcess->entityId);

  if (eventToProcess->type == ARRIVAL)
  {
    if (involvedEntity != nullptr)
      involvedEntity->state = WAITING;
  }
  else if (eventToProcess->type == SERVICE_START)
  {
    if (involvedEntity != nullptr)
    {
      involvedEntity->state = BUSY;
      involvedEntity->timeServiceStarted = eventToProcess->time;

      double waitTime = involvedEntity->getWaitTime();
      if (waitTime >= 0)
      {
        currentStats.totalWaitTime += waitTime;

        if (waitTime > currentStats.longestWaitTime)
          currentStats.longestWaitTime = waitTime;

        if (waitTime < currentStats.shortestWaitTime)
          currentStats.shortestWaitTime = waitTime;
      }
    }
  }
  else if (eventToProcess->type == DEPARTURE)
  {
    if (involvedEntity != nullptr)
    {
      involvedEntity->state = FINISHED;
      involvedEntity->timeOfDeparture = eventToProcess->time;
      currentStats.totalEntitiesFinished++;
    }
  }
  else if (eventToProcess->type == ESCALATION)
  {
    if (involvedEntity != nullptr && involvedEntity->state == WAITING)
    {
      if (involvedEntity->priorityLevel == 5)
        involvedEntity->priorityLevel = 3;
      else if (involvedEntity->priorityLevel == 3)
        involvedEntity->priorityLevel = 1;
    }
  }
  else if (eventToProcess->type == SIGNAL_CHECK)
  {
    // handled by TrafficSim
  }
  else if (eventToProcess->type == TICK)
  {
    // periodic tick
  }
}

// reset for another run
void SimEngine::reset()
{
  simulationClock->reset();
  currentStats = SimulationStats();
  engineIsRunning = false;
}

// print results
void SimEngine::printStats() const
{
  std::cout << std::endl;
  std::cout << "       SIMULATION RESULTS" << std::endl;
  std::cout << "Scenario: " << currentStats.scenarioName << std::endl;
  std::cout << "Total entities created:   " << currentStats.totalEntitiesCreated << std::endl;
  std::cout << "Total entities finished:  " << currentStats.totalEntitiesFinished << std::endl;
  std::cout << "Total events processed:   " << currentStats.totalEventsProcessed << std::endl;
  std::cout << "Average wait time:        " << currentStats.getAverageWaitTime() << " seconds" << std::endl;
  std::cout << "Longest wait time:        " << currentStats.longestWaitTime << " seconds" << std::endl;
  std::cout << std::endl;
}