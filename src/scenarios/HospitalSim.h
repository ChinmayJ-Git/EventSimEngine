#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "../core/DynamicArray.h"
#include "../engine/SimEngine.h"

#include <string>

class HospitalSim : public EventHandler
{
private:
    SimEngine *engine;
    int numberOfDoctors;
    int numberOfBeds;
    double duration;
    double arrivalGap;

    DynamicArray<int> doctorIds;
    DynamicArray<int> bedIds;
    DynamicArray<int> waitingPatientIds;

    int nextEntityId;

    int totalPatients;
    int totalServed;
    int totalEscalations;
    double totalWaitTime;
    double longestWait;

public:
    HospitalSim(SimEngine *engine, int numberOfDoctors,
                int numberOfBeds, double duration,
                double arrivalGap);

    void initialise();
    void run();
    void printResults();
    void onEvent(Event *e);

private:
    void handleArrival(Event *eventData);
    void handleServiceStart(Event *eventData);
    void handleServiceEnd(Event *eventData);
    void handleEscalation(Event *eventData);
    void handleDeparture(Event *eventData);

    int findFreeDoctor() const;
    int findWaitingIndexById(int patientId) const;
    void insertWaitingPatient(int patientId);
    void removeWaitingAt(int index);
    void sortWaitingByPriority();

    int getRandomPriority() const;
    double getRandomArrivalGap() const;
    double getRandomServiceDuration() const;
};

#endif