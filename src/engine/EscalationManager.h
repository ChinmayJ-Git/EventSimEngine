#ifndef ESCALATIONMANAGER_H
#define ESCALATIONMANAGER_H

#include "Entity.h"
#include "SimEngine.h"

// time thresholds before priority upgrades
static const double MINOR_ESCALATION_WAIT = 60.0;   // minor → normal after 60s
static const double NORMAL_ESCALATION_WAIT = 120.0; // normal → critical after 120s

class EscalationManager
{
public:
  // schedule escalation event for a newly arrived patient
  static void scheduleEscalation(Entity *patient, SimEngine *engine)
  {
    // only patients need escalation
    if (patient->type != PATIENT)
      return;

    // critical patients do not escalate — already highest
    if (patient->priorityLevel == 1)
      return;

    double escalationTime = -1.0;

    // minor patient escalates after MINOR_ESCALATION_WAIT seconds
    if (patient->priorityLevel == 5)
      escalationTime = patient->timeOfArrival + MINOR_ESCALATION_WAIT;

    // normal patient escalates after NORMAL_ESCALATION_WAIT seconds
    if (patient->priorityLevel == 3)
      escalationTime = patient->timeOfArrival + NORMAL_ESCALATION_WAIT;

    if (escalationTime > 0)
    {
      // store deadline on the entity itself
      patient->escalationDeadline = escalationTime;

      // schedule ESCALATION event on the engine
      engine->scheduleEvent(escalationTime, ESCALATION, patient->id,
                            patient->currentLocationId,
                            patient->name + " priority escalates");
    }
  }

  // process an escalation event — upgrade the patient's priority
  static void processEscalation(Entity *patient, SimEngine *engine)
  {
    // if patient is already being served or finished, ignore
    if (patient->state == BUSY || patient->state == FINISHED)
      return;

    // upgrade priority
    if (patient->priorityLevel == 5)
    {
      // minor becomes normal
      patient->priorityLevel = 3;

      // schedule another escalation — now as normal patient
      double nextEscalation = engine->getCurrentTime() + NORMAL_ESCALATION_WAIT;
      patient->escalationDeadline = nextEscalation;
      engine->scheduleEvent(nextEscalation, ESCALATION, patient->id,
                            patient->currentLocationId,
                            patient->name + " escalates again to critical");
    }
    else if (patient->priorityLevel == 3)
    {
      // normal becomes critical
      patient->priorityLevel = 1;

      // no further escalation — already critical
      patient->escalationDeadline = -1.0;
    }
  }
};

#endif