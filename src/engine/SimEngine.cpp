// This file implements the logic of the Simulation Engine.

#include "SimEngine.h"
#include <iostream>

//set up the engine before any simulation starts
SimEngine::SimEngine(double startTime, double endTime, bool showDetails)
{
  simulationClock = new SimClock(startTime, endTime);
  engineIsRunning = false;
  verboseMode = showDetails;
  currentStats = SimulationStats();
}
SimEngine::~SimEngine()
{

  delete simulationClock;
}

void SimEngine::scheduleEvent(double eventTime, EventType type, int entityId,
                              int locationId, std::string description)
{

  Event *newEvent = new Event(eventTime, type, entityId, locationId, description);

  eventQueue.insert(newEvent);
}

// addEntity to register a new entity in the simulation
void SimEngine::addEntity(Entity *newEntity)
{

  entityTable.addEntity(newEntity);

  currentStats.totalEntitiesCreated++;
}

Entity *SimEngine::getEntity(int entityId)
{
  return entityTable.findEntityById(entityId);
}

double SimEngine::getCurrentTime() const
{
  return simulationClock->getCurrentTime();
}

SimulationStats SimEngine::getStats() const
{
  return currentStats;
}

void SimEngine::run()
{
  engineIsRunning = true;

  std::cout << "Simulation started." << std::endl;
  std::cout << "Running until time: " << simulationClock->getEndTime() << std::endl;

  // Keep going as long as:
  // 1. There are still events waiting to be processed
  // 2. The simulation clock has not hit the end time
  while (!eventQueue.isEmpty() && !simulationClock->hasSimulationEnded())
  {
    Event *nextEvent = eventQueue.extractMinimum();
    if (nextEvent == nullptr)
    {
      break;
    }
    simulationClock->advance(nextEvent->time);

    if (simulationClock->hasSimulationEnded())
    {
      delete nextEvent;
      break;
    }
    if (verboseMode)
    {
      std::cout << "Time " << nextEvent->time
                << " | Event: " << nextEvent->description
                << " | Entity ID: " << nextEvent->entityId
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

void SimEngine::processEvent(Event *eventToProcess)
{
  Entity *involvedEntity = entityTable.findEntityById(eventToProcess->entityId);

  if (eventToProcess->type == ARRIVAL)
  {
    if (involvedEntity != nullptr)
    {
      involvedEntity->state = WAITING;
    }
  }
  else if (eventToProcess->type == SERVICE_START)
  {
    if (involvedEntity != nullptr)
    {
      involvedEntity->state = BUSY;

      involvedEntity->timeServiceStarted = eventToProcess->time;

      double thisEntitysWaitTime = involvedEntity->getWaitTime();
      if (thisEntitysWaitTime >= 0)
      {

        currentStats.totalWaitTime += thisEntitysWaitTime;

        if (thisEntitysWaitTime > currentStats.longestWaitTime)
        {
          currentStats.longestWaitTime = thisEntitysWaitTime;
        }

        if (thisEntitysWaitTime < currentStats.shortestWaitTime)
        {
          currentStats.shortestWaitTime = thisEntitysWaitTime;
        }
      }
    }
  }
  else if (eventToProcess->type == DEPARTURE)
  {
    if (involvedEntity != nullptr)
    {
      involvedEntity->state = FINISHED;
      involvedEntity->timeOfDeparture = eventToProcess->time;

      // Count one more finished entity
      currentStats.totalEntitiesFinished++;
    }
  }
  else if (eventToProcess->type == TICK)
  {
  }
  else
  {
  }
}

// clear everything for another simulation run

void SimEngine::reset()
{

  simulationClock->reset();
  currentStats = SimulationStats();
  engineIsRunning = false;
}

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