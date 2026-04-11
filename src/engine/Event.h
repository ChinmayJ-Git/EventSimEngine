// This file describes what a single "event" looks like in our simulation.
//  Concept: This is the data that goes into our Priority Queue (Min-Heap).

#ifndef EVENT_H
#define EVENT_H

#include <string>

// event types
enum EventType
{
    ARRIVAL,
    DEPARTURE,
    SERVICE_START,
    SERVICE_END,
    SIGNAL_CHANGE,
    SIGNAL_CHECK,
    ESCALATION,
    TICK,
    CUSTOM
};

// one event — a moment in time
struct Event
{
    double time;
    EventType type;
    int entityId;
    int locationId;
    std::string description;

    // constructor
    Event(double eventTime, EventType eventType, int whoIsInvolved,
          int whereItHappens, std::string desc = "")
    {
        time = eventTime;
        type = eventType;
        entityId = whoIsInvolved;
        locationId = whereItHappens;
        description = desc;
    }

    // comparisons for min-heap
    bool operator<(const Event& other) const
    {
        return time < other.time;
    }

    bool operator>(const Event& other) const
    {
        return time > other.time;
    }

    bool operator==(const Event& other) const
    {
        return time == other.time;
    }
};

#endif