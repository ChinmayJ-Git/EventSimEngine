#include "HospitalSim.h"
#include <iostream>

// constructor
HospitalSim::HospitalSim(int doctors, int beds, double escalationLimit) {
    totalDoctors = doctors;
    availableDoctors = doctors;

    totalBeds = beds;
    availableBeds = beds;

    escalationTimeLimit = escalationLimit;
}

// process event
void HospitalSim::processEvent(Event event) {

    Entity* patient = event.Entity;
    double currentTime = event.time;

    if (event.type == ARRIVAL) {
        handleArrival(patient, currentTime);
    }
    else if (event.type == SERVICE_START) {
        handleTreatmentStart(patient, currentTime);
    }
    else if (event.type == SERVICE_END) {
        handleTreatmentEnd(patient, currentTime);
    }
    else if (event.type == ESCALATION) {
        handleEscalation(patient, currentTime);
    }
}

// arrival
void HospitalSim::handleArrival(Entity* patient, double currentTime) {
    std::cout << "Patient arrived\n";
}

// start treatment
void HospitalSim::handleTreatmentStart(Entity* patient, double currentTime) {
    std::cout << "Treatment started\n";
}

// end treatment
void HospitalSim::handleTreatmentEnd(Entity* patient, double currentTime) {
    std::cout << "Treatment ended\n";
}

// escalation
void HospitalSim::handleEscalation(Entity* patient, double currentTime) {
    std::cout << "Patient escalated\n";
}