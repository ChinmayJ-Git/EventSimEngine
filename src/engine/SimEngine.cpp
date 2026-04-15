#include "SimEngine.h"

#include <windows.h>

SimEngine::SimEngine(double endTime)
    : clock(endTime)
{
  nextEventId = 1;
  nextPatientId = 1;
  eventHandler = 0;
  eventDelayMs = 500;
}

void SimEngine::setEventHandler(EventHandler *handler)
{
  eventHandler = handler;
}

void SimEngine::setFastMode(bool fastMode)
{
  eventDelayMs = fastMode ? 100 : 500;
}
void SimEngine::scheduleEvent(EventType type, double time, int patientId, std::string doctorType)
{
  Event *e = new Event();
  e->id = nextEventId;
  nextEventId++;
  e->type = type;
  e->time = time;
  e->patientId = patientId;
  e->doctorType = doctorType;
  eventQueue.insert(e);
}
void SimEngine::addPatient(Patient *p)
{
  if (p == 0)
  {
    return;
  }
  if (p->id < 0)
  {
    p->id = nextPatientId;
    nextPatientId++;
  }
  entityTable.insert(p->id, p);
}
Patient *SimEngine::getPatient(int id)
{
  return entityTable.get(id);
}

double SimEngine::getCurrentTime() const
{
  return clock.getCurrentTime();
}

void SimEngine::run()
{
  while (!clock.hasSimulationEnded() && !eventQueue.isEmpty())
  {
    Event *e = eventQueue.extractMinimum();
    if (e == 0)
    {
      break;
    }
    clock.advance(e->time);
    processEvent(e);
    if (eventHandler != 0)
    {
      eventHandler->onEvent(e);
    }
    delete e;
    Sleep((DWORD)eventDelayMs);
  }
}
void SimEngine::processEvent(Event *e)
{
  if (e == 0)
  {
    return;
  }

  switch (e->type)
  {
  case ARRIVAL:
  {
    Patient *p = getPatient(e->patientId);
    if (p != 0)
    {
      p->state = WAITING;
    }
    break;
  }
  case SERVICE_START:
  {
    Patient *p = getPatient(e->patientId);
    if (p != 0)
    {
      p->state = BUSY;
      p->serviceStartTime = e->time;
      p->assignedDoctorType = e->doctorType;
    }
    break;
  }
  case SERVICE_END:
  {
    Patient *p = getPatient(e->patientId);
    if (p != 0)
    {
      p->state = FINISHED;
      p->departureTime = e->time;
    }
    break;
  }
  case ESCALATION:
  {
    break;
  }
  }
}