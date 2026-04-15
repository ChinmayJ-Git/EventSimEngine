#include "SimEngine.h"
#include <iostream>

// set up engine
SimEngine::SimEngine(double startTime, double endTime, bool showDetails)
{
  simulationClock = new SimClock(startTime, endTime);
  engineIsRunning = false;
  verboseMode = showDetails;
  currentStats = SimulationStats();
  scenarioHandler = nullptr;
}

// clean up memory
SimEngine::~SimEngine()
{
  delete simulationClock;
}

// register scenario handler
void SimEngine::setEventHandler(EventHandler *handler)
{
  scenarioHandler = handler;
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

// process one event — used by step by step loop
bool SimEngine::processOneEvent()
{
  if (eventQueue.isEmpty() || simulationClock->hasSimulationEnded())
    return false;

  Event *nextEvent = eventQueue.extractMinimum();
  if (nextEvent == nullptr)
    return false;

  simulationClock->advance(nextEvent->time);

  if (simulationClock->hasSimulationEnded())
  {
    delete nextEvent;
    return false;
  }

  // print event to console
  // only print important events — skip signal checks and minor ticks
  if (nextEvent->type != SIGNAL_CHECK && nextEvent->type != TICK)
    {
    std::cout << "[t=" << (int)nextEvent->time << "] "
              << nextEvent->description << std::endl;
    }

  // update entity states
  processEvent(nextEvent);

  // call scenario handler so hospital/traffic logic fires
  if (scenarioHandler != nullptr)
    scenarioHandler->onEvent(nextEvent);

  currentStats.totalEventsProcessed++;
  delete nextEvent;

  if (eventQueue.isEmpty() || simulationClock->hasSimulationEnded())
    return false;

  return true;
}

// main simulation loop — runs everything at once
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

    processEvent(nextEvent);

    if (scenarioHandler != nullptr)
      scenarioHandler->onEvent(nextEvent);

    currentStats.totalEventsProcessed++;
    delete nextEvent;
  }

  engineIsRunning = false;
  std::cout << "Simulation complete." << std::endl;
  std::cout << "Total events processed: " << currentStats.totalEventsProcessed << std::endl;
}

// handle entity state changes for one event
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
    // handled by TrafficSim via callback
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