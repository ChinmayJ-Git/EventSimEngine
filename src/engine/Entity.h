// This file describes what an "entity" is in our simulation.
// Concept: Struct / object — the data nodes in our Graph and HashMap.

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
enum EntityType
{
    PATIENT,        
    DOCTOR,         
    BED,            
    ROUTER,         
    PACKET,         
    CAR,            
    TRAFFIC_LIGHT,  
    GENERIC         
};

enum EntityState
{
    IDLE,       
    BUSY,      
    WAITING,    
    MOVING,     
    FINISHED    
};


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
    }

    double getWaitTime() const
    {
        if (timeServiceStarted < 0)
        {
            return -1.0;
        }
        double waitTime = timeServiceStarted - timeOfArrival;
        return waitTime;
    }

    
// This function returns the time from arrival to departure
    double getTotalTimeInSystem() const
    {
        // If they have not left yet, we cannot calculate this
        if (timeOfDeparture < 0)
        {
            return -1.0;
        }

        // Total time = moment they left minus moment they arrived
        double totalTime = timeOfDeparture - timeOfArrival;
        return totalTime;
    }
};

#endif