#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "Event.h"

// hospital simulation
class HospitalSim {

private:
    int totalDoctors;
    int availableDoctors;

    double escalationTimeLimit;

    // patient data
    int patientIds[100];
    int patientPriority[100];     // 1 = critical, 3 = normal, 5 = minor
    double arrivalTime[100];

    int waitingCount;

    // stats
    double totalWaitTime;
    int totalPatients;

public:

    // constructor
    HospitalSim(int doctors, double escalationLimit);

    // process event
    void processEvent(Event event);

    // handlers
    void handleArrival(int patientId, double currentTime);
    void handleTreatmentStart(int patientId, double currentTime);
    void handleTreatmentEnd(int patientId, double currentTime);
    void handleEscalation(int patientId, double currentTime);

    // helper
    int getHighestPriorityPatient();

};

#endif