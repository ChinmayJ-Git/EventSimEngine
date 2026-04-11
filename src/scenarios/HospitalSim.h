#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "src/core/DynamicArray.h"
#include "src/core/MinHeap.h"
#include "src/engine/SimEngine.h"

#include <string>

class HospitalSim
{
private:
    SimEngine *engine;
    int numberOfDoctors;
    int numberOfBeds;
    double duration;

    MinHeap localEventQueue;
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
                int numberOfBeds, double duration);

    void initialise();
    void run();
    void printResults();

private:
    void queueScenarioEvent(double eventTime, EventType type, int entityId,
                            int locationId, const std::string &description);
    void processScenarioLogic();

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