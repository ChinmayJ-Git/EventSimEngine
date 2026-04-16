#ifndef SIMENGINE_H
#define SIMENGINE_H

#include "../core/MinHeap.h"
#include "../core/HashMap.h"
#include "SimClock.h"
#include "Event.h"
#include "Entity.h"

#include <string>

class EventHandler
{
public:
  virtual void onEvent(Event *e) = 0;
  virtual ~EventHandler() {}
};

class SimEngine
{
private:
  MinHeap eventQueue;
  HashMap entityTable;
  SimClock clock;
  int nextEventId;
  int nextPatientId;
  EventHandler *eventHandler;
  int eventDelayMs;
  bool stopRequested;

public:
  SimEngine(double endTime);
  void setEventHandler(EventHandler *handler);
  void setFastMode(bool fastMode);
  void scheduleEvent(EventType type, double time, int patientId, std::string doctorType);
  void addPatient(Patient *p);
  Patient *getPatient(int id);
  double getCurrentTime() const;
  void requestStop();
  void run();
  void processEvent(Event *e);
};

#endif