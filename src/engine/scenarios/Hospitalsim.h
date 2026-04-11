#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "../engine/Event.h"
#include "../engine/Entity.h"

// hospital simulation
class HospitalSim {

private:
    int totalDoctors;
    int availableDoctors;

    int totalBeds;
    int availableBeds;

    double escalationTimeLimit;

public:

    // constructor
    HospitalSim(int doctors, int beds, double escalationLimit);

    // process event
    void processEvent(Event event);

    // arrival
    void handleArrival(Entity* patient, double currentTime);

    // start treatment
    void handleTreatmentStart(Entity* patient, double currentTime);

    // end treatment
    void handleTreatmentEnd(Entity* patient, double currentTime);

    // escalation
    void handleEscalation(Entity* patient, double currentTime);

};

#endif