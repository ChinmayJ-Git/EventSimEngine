#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "Event.h"

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
    void handleArrival(int patientId, double currentTime);

    // start treatment
    void handleTreatmentStart(int patientId, double currentTime);

    // end treatment
    void handleTreatmentEnd(int patientId, double currentTime);

    // escalation
    void handleEscalation(int patientId, double currentTime);

};

#endif