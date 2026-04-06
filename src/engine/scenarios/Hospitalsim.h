// HospitalSim.h
// Written by: Sai
// What this file does: Defines hospital simulation structure
// DSA concept used: Queue, Priority Queue

#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include <iostream>
#include <string>

using namespace std;

// This class represents the hospital simulation
class HospitalSim {
    
private:
    int numberOfDoctors;     // total doctors available
    int numberOfBeds;        // total beds available
    int patientsWaiting;     // number of patients waiting

public:

    // Constructor: sets initial values
    HospitalSim(int doctors, int beds) {
        numberOfDoctors = doctors;   // set doctors count
        numberOfBeds = beds;         // set beds count
        patientsWaiting = 0;         // initially no patients waiting
    }

    // Function when a patient arrives
    void patientArrives() {
        cout << "Patient arrived" << endl;
        patientsWaiting++;   // increase waiting count
    }

    // Function to start treatment
    void startTreatment() {
        if (patientsWaiting > 0) {
            cout << "Doctor started treating patient" << endl;
            patientsWaiting--;   // one patient goes for treatment
        }
        else {
            cout << "No patients waiting" << endl;
        }
    }

    // Function to end treatment
    void endTreatment() {
        cout << "Treatment finished, patient leaves" << endl;
    }

};

#endif