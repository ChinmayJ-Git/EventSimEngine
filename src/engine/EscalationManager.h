#ifndef ESCALATIONMANAGER_H
#define ESCALATIONMANAGER_H

#include "SimEngine.h"
#include "Entity.h"

class EscalationManager
{
public:
  static void scheduleEscalation(Patient *p, SimEngine *engine)
  {
    if (p == 0 || engine == 0)
    {
      return;
    }
    double waitSeconds = 0.0;
    if (p->priority == 3)
    {
      waitSeconds = 90.0;
    }
    else if (p->priority == 2)
    {
      waitSeconds = 60.0;
    }
    else
    {
      p->escalationDeadline = -1.0;
      return;
    }
    double baseTime = p->arrivalTime;
    if (p->escalationDeadline > baseTime)
    {
      baseTime = p->escalationDeadline;
    }
    p->escalationDeadline = baseTime + waitSeconds;
    engine->scheduleEvent(ESCALATION, p->escalationDeadline, p->id, "");
  }

  static void processEscalation(Patient *p, SimEngine *engine)
  {
    if (p == 0 || engine == 0)
    {
      return;
    }
    if (p->state != WAITING)
    {
      return;
    }
    if (p->priority == 3)
    {
      p->priority = 2;
    }
    else if (p->priority == 2)
    {
      p->priority = 1;
    }
    else
    {
      p->escalationDeadline = -1.0;
      return;
    }
    scheduleEscalation(p, engine);
  }
};

#endif