#ifndef EVENT_H
#define EVENT_H

#include <string>

// event categories
enum EventType
{
    ARRIVAL,
    SERVICE_START,
    SERVICE_END,
    ESCALATION
};

// event data bundle
struct Event
{
    int id;
    EventType type;
    double time;
    int patientId;
    std::string doctorType;

    Event()
    {
        id = -1;
        type = ARRIVAL;
        time = 0.0;
        patientId = -1;
        doctorType = "";
    }

    Event(int eventId, EventType eventType, double eventTime,
          int eventPatientId, const std::string &eventDoctorType)
    {
        id = eventId;
        type = eventType;
        time = eventTime;
        patientId = eventPatientId;
        doctorType = eventDoctorType;
    }

    // compare by time
    bool operator<(const Event &other) const
    {
        return time < other.time;
    }
};

#endif