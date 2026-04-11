#include "HospitalSim.h"
#include <iostream>

// constructor
HospitalSim::HospitalSim(int doctors, double escalationLimit) {
    totalDoctors = doctors;
    availableDoctors = doctors;
    escalationTimeLimit = escalationLimit;

    waitingCount = 0;

    totalWaitTime = 0;
    totalPatients = 0;
}

// process event
void HospitalSim::processEvent(Event event) {

    int patientId = event.entityId;
    double currentTime = event.time;

    if (event.type == ARRIVAL) {
        handleArrival(patientId, currentTime);
    }
    else if (event.type == SERVICE_START) {
        handleTreatmentStart(patientId, currentTime);
    }
    else if (event.type == SERVICE_END) {
        handleTreatmentEnd(patientId, currentTime);
    }
    else if (event.type == ESCALATION) {
        handleEscalation(patientId, currentTime);
    }
}

// arrival
void HospitalSim::handleArrival(int patientId, double currentTime) {

    std::cout << "Patient arrived: " << patientId << "\n";

    // assign default priority (minor)
    patientIds[waitingCount] = patientId;
    patientPriority[waitingCount] = 5;
    arrivalTime[waitingCount] = currentTime;

    waitingCount++;
    totalPatients++;

    // if doctor available → treat immediately
    if (availableDoctors > 0) {
        availableDoctors--;
        handleTreatmentStart(patientId, currentTime);
    }
}

// start treatment
void HospitalSim::handleTreatmentStart(int patientId, double currentTime) {

    std::cout << "Treatment started: " << patientId << "\n";

    // calculate wait time
    for (int i = 0; i < waitingCount; i++) {
        if (patientIds[i] == patientId) {
            double wait = currentTime - arrivalTime[i];
            totalWaitTime += wait;

            // remove patient from queue
            for (int j = i + 1; j < waitingCount; j++) {
                patientIds[j - 1] = patientIds[j];
                patientPriority[j - 1] = patientPriority[j];
                arrivalTime[j - 1] = arrivalTime[j];
            }
            waitingCount--;
            break;
        }
    }
}

// end treatment
void HospitalSim::handleTreatmentEnd(int patientId, double currentTime) {

    std::cout << "Treatment ended: " << patientId << "\n";

    availableDoctors++;

    // assign next highest priority patient
    if (waitingCount > 0) {
        int index = getHighestPriorityPatient();
        int nextPatient = patientIds[index];

        availableDoctors--;
        handleTreatmentStart(nextPatient, currentTime);
    }
}

// escalation
void HospitalSim::handleEscalation(int patientId, double currentTime) {

    std::cout << "Patient escalated: " << patientId << "\n";

    for (int i = 0; i < waitingCount; i++) {
        if (patientIds[i] == patientId) {

            // upgrade priority
            if (patientPriority[i] == 5) {
                patientPriority[i] = 3;
            }
            else if (patientPriority[i] == 3) {
                patientPriority[i] = 1;
            }

            break;
        }
    }
}

// get highest priority patient
int HospitalSim::getHighestPriorityPatient() {

    int bestIndex = 0;

    for (int i = 1; i < waitingCount; i++) {
        if (patientPriority[i] < patientPriority[bestIndex]) {
            bestIndex = i;
        }
    }

    return bestIndex;
}