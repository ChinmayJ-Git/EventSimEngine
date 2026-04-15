#ifndef ENTITY_H
#define ENTITY_H

#include <string>

enum EntityType
{
    PATIENT,
    DOCTOR
};

enum EntityState
{
    IDLE,
    WAITING,
    BUSY,
    FINISHED
};

struct Patient
{
    int id;
    std::string name;
    int priority;
    double arrivalTime;
    double serviceStartTime;
    double departureTime;
    EntityState state;
    std::string assignedDoctorType;
    double escalationDeadline;

    Patient()
    {
        id = -1;
        name = "";
        priority = 3;
        arrivalTime = 0.0;
        serviceStartTime = -1.0;
        departureTime = -1.0;
        state = WAITING;
        assignedDoctorType = "";
        escalationDeadline = -1.0;
    }
};

struct Doctor
{
    int id;
    std::string specialization;
    bool isAvailable;
    int totalPatientsSeen;

    Doctor()
    {
        id = -1;
        specialization = "";
        isAvailable = true;
        totalPatientsSeen = 0;
    }
};

#endif