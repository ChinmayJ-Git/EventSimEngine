// This file describes what a single "event" looks like in our simulation.
//  Concept: This is the data that goes into our Priority Queue (Min-Heap).

#ifndef EVENT_H
#define EVENT_H

#include <string>
enum EventType
{
  ARRIVAL,       // (patient walks in, packet enters router)
  DEPARTURE,     // (patient goes home, packet exits router)
  SERVICE_START, // (doctor starts treating the patient)
  SERVICE_END,   // (doctor finishes, patient ready to leave)
  SIGNAL_CHANGE, // (green becomes red, etc.)
  PACKET_SEND,   // network simulation (router sends a packet forward)
  TICK,          // a regular update every few seconds (for animations)
  CUSTOM         // any special event we might add in a specific scenario
};

struct Event
{
  double time;
  EventType type;
  int entityId;
  int locationId;
  std::string description;

  Event(double eventTime, EventType eventType, int whoIsInvolved,
        int whereItHappens, std::string desc = "")
  {
    time = eventTime;
    type = eventType;
    entityId = whoIsInvolved;
    locationId = whereItHappens;
    description = desc;
  }

  bool operator<(const Event &otherEvent) const
  {
    return time < otherEvent.time;
  }
  bool operator>(const Event &otherEvent) const
  {
    return time > otherEvent.time;
  }
  bool operator==(const Event &otherEvent) const
  {
    return time == otherEvent.time;
  }
};

#endif