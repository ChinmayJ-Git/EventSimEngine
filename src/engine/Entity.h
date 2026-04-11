#ifndef ENTITY_H
#define ENTITY_H

#include <string>

// entity types
enum EntityType
{
    PATIENT,
    DOCTOR,
    BED,
    CAR,
    TRAFFIC_LIGHT,
    GENERIC
};

// entity states
enum EntityState
{
    IDLE,
    BUSY,
    WAITING,
    MOVING,
    FINISHED
};

// one entity in the simulation
struct Entity
{
    int id;
    EntityType type;
    EntityState state;
    std::string name;
    double timeOfArrival;
    double timeServiceStarted;
    double timeOfDeparture;
    int priorityLevel;
    int currentLocationId;

    // triage escalation deadline
    // if patient is not seen before this time, ESCALATION event fires
    // -1 means no escalation scheduled
    double escalationDeadline;

    // constructor
    Entity(int entityId, EntityType entityType, std::string entityName,
           double arrivalTime, int priority = 5)
    {
        id = entityId;
        type = entityType;
        state = WAITING;
        name = entityName;
        timeOfArrival = arrivalTime;
        timeServiceStarted = -1.0;
        timeOfDeparture = -1.0;
        priorityLevel = priority;
        currentLocationId = -1;
        escalationDeadline = -1.0;
    }

    // calc wait time
    double getWaitTime() const
    {
        if (timeServiceStarted < 0)
        {
            return -1.0;
        }
        return timeServiceStarted - timeOfArrival;
    }

    // calc total time in system
    double getTotalTimeInSystem() const
    {
        if (timeOfDeparture < 0)
        {
            return -1.0;
        }
        return timeOfDeparture - timeOfArrival;
    }
};

#endif