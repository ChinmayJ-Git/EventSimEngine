#include "HospitalSim.h"

#include "../engine/EscalationManager.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

static bool hospitalSeeded = false;

HospitalSim::HospitalSim(SimEngine *engine, int numberOfDoctors,
                         int numberOfBeds, double duration,
                         double arrivalGap)
{
    this->engine = engine;
    this->numberOfDoctors = numberOfDoctors;
    this->numberOfBeds = numberOfBeds;
    this->duration = duration;
    this->arrivalGap = arrivalGap;

    if (this->arrivalGap <= 0.0)
    {
        this->arrivalGap = 1.0;
    }

    nextEntityId = 10000;

    totalPatients = 0;
    totalServed = 0;
    totalEscalations = 0;
    totalWaitTime = 0.0;
    longestWait = 0.0;

    if (!hospitalSeeded)
    {
        std::srand((unsigned int)std::time(nullptr));
        hospitalSeeded = true;
    }
}

// route events to correct handler
void HospitalSim::onEvent(Event *e)
{
    if (e->type == ARRIVAL)
        handleArrival(e);
    else if (e->type == SERVICE_START)
        handleServiceStart(e);
    else if (e->type == SERVICE_END)
        handleServiceEnd(e);
    else if (e->type == ESCALATION)
        handleEscalation(e);
    else if (e->type == DEPARTURE)
        handleDeparture(e);
}

void HospitalSim::initialise()
{
    engine->setEventHandler(this);
    for (int i = 0; i < numberOfDoctors; i++)
    {
        int doctorId = nextEntityId;
        nextEntityId++;

        std::string doctorName = "Doctor_" + std::to_string(i + 1);
        Entity *doctor = new Entity(doctorId, DOCTOR, doctorName, 0.0, 1);
        doctor->state = IDLE;
        doctor->currentLocationId = i;

        engine->addEntity(doctor);
        doctorIds.pushBack(doctorId);
    }

    for (int i = 0; i < numberOfBeds; i++)
    {
        int bedId = nextEntityId;
        nextEntityId++;

        std::string bedName = "Bed_" + std::to_string(i + 1);
        Entity *bed = new Entity(bedId, BED, bedName, 0.0, 1);
        bed->state = IDLE;
        bed->currentLocationId = i;

        engine->addEntity(bed);
        bedIds.pushBack(bedId);
    }

    double firstTime = 1.0;
    for (int i = 0; i < 3; i++)
    {
        firstTime = firstTime + getRandomArrivalGap();
        if (firstTime <= duration)
        {
            engine->scheduleEvent(firstTime, ARRIVAL, 0, 0, "patient arrives");
        }
    }
}

void HospitalSim::run()
{
    engine->run();
}

void HospitalSim::printResults()
{
    double averageWait = 0.0;
    if (totalServed > 0)
    {
        averageWait = totalWaitTime / totalServed;
    }

    std::cout << "Hospital results" << std::endl;
    std::cout << "Patients total: " << totalPatients << std::endl;
    std::cout << "Patients served: " << totalServed << std::endl;
    std::cout << "Escalations total: " << totalEscalations << std::endl;
    std::cout << "Average wait: " << averageWait << std::endl;
    std::cout << "Longest wait: " << longestWait << std::endl;
}

void HospitalSim::handleArrival(Event *eventData)
{
    int patientId = nextEntityId;
    nextEntityId++;

    int priority = getRandomPriority();
    std::string patientName = "Patient_" + std::to_string(patientId);
    Entity *patient = new Entity(patientId, PATIENT, patientName, eventData->time, priority);
    patient->currentLocationId = 0;

    engine->addEntity(patient);
    totalPatients++;

    EscalationManager::scheduleEscalation(patient, engine);
    if (patient->escalationDeadline > 0.0 && patient->escalationDeadline <= duration)
    {
        engine->scheduleEvent(patient->escalationDeadline, ESCALATION,
                              patient->id, 0, "patient escalates");
    }

    int doctorId = findFreeDoctor();
    if (doctorId != -1)
    {
        engine->scheduleEvent(eventData->time, SERVICE_START,
                              patient->id, doctorId, "service starts");
    }
    else
    {
        insertWaitingPatient(patient->id);
    }

    double nextArrival = eventData->time + getRandomArrivalGap();
    if (nextArrival <= duration)
    {
        engine->scheduleEvent(nextArrival, ARRIVAL, 0, 0, "patient arrives");
    }
}

void HospitalSim::handleServiceStart(Event *eventData)
{
    Entity *doctor = engine->getEntity(eventData->locationId);
    Entity *patient = engine->getEntity(eventData->entityId);

    if (doctor == nullptr || patient == nullptr)
    {
        return;
    }

    doctor->state = BUSY;
    patient->state = BUSY;
    patient->timeServiceStarted = eventData->time;

    int waitingIndex = findWaitingIndexById(patient->id);
    if (waitingIndex != -1)
    {
        double waitTime = eventData->time - patient->timeOfArrival;
        totalWaitTime = totalWaitTime + waitTime;
        if (waitTime > longestWait)
        {
            longestWait = waitTime;
        }

        removeWaitingAt(waitingIndex);
    }

    double serviceDuration = getRandomServiceDuration();
    engine->scheduleEvent(eventData->time + serviceDuration, SERVICE_END,
                          patient->id, doctor->id, "service ends");
}

void HospitalSim::handleServiceEnd(Event *eventData)
{
    Entity *doctor = engine->getEntity(eventData->locationId);
    Entity *patient = engine->getEntity(eventData->entityId);

    if (doctor != nullptr)
    {
        doctor->state = IDLE;
    }

    if (patient != nullptr)
    {
        engine->scheduleEvent(eventData->time + 5.0, DEPARTURE,
                              patient->id, eventData->locationId, "patient leaves");
    }

    if (!waitingPatientIds.isEmpty())
    {
        int nextPatientId = waitingPatientIds.get(0);
        engine->scheduleEvent(eventData->time, SERVICE_START,
                              nextPatientId, eventData->locationId, "service starts");
    }
}

void HospitalSim::handleEscalation(Event *eventData)
{
    Entity *patient = engine->getEntity(eventData->entityId);
    if (patient == nullptr)
    {
        return;
    }

    int oldPriority = patient->priorityLevel;
    EscalationManager::processEscalation(patient, engine);

    if (patient->priorityLevel != oldPriority)
    {
        totalEscalations++;
        sortWaitingByPriority();
    }

    if (patient->state == WAITING)
    {
        if (patient->escalationDeadline > eventData->time &&
            patient->escalationDeadline <= duration)
        {
            engine->scheduleEvent(patient->escalationDeadline, ESCALATION,
                                  patient->id, 0, "patient escalates");
        }
    }
}

void HospitalSim::handleDeparture(Event *eventData)
{
    Entity *patient = engine->getEntity(eventData->entityId);
    if (patient == nullptr)
    {
        return;
    }

    patient->state = FINISHED;
    patient->timeOfDeparture = eventData->time;
    totalServed++;
}

int HospitalSim::findFreeDoctor() const
{
    for (int i = 0; i < doctorIds.size(); i++)
    {
        int doctorId = doctorIds.get(i);
        Entity *doctor = engine->getEntity(doctorId);
        if (doctor != nullptr && doctor->state == IDLE)
        {
            return doctorId;
        }
    }

    return -1;
}

int HospitalSim::findWaitingIndexById(int patientId) const
{
    for (int i = 0; i < waitingPatientIds.size(); i++)
    {
        if (waitingPatientIds.get(i) == patientId)
        {
            return i;
        }
    }

    return -1;
}

void HospitalSim::insertWaitingPatient(int patientId)
{
    Entity *newPatient = engine->getEntity(patientId);
    if (newPatient == nullptr)
    {
        return;
    }

    int oldSize = waitingPatientIds.size();
    waitingPatientIds.pushBack(patientId);

    int insertIndex = oldSize;
    for (int i = 0; i < oldSize; i++)
    {
        Entity *queuedPatient = engine->getEntity(waitingPatientIds.get(i));
        if (queuedPatient != nullptr)
        {
            if (newPatient->priorityLevel < queuedPatient->priorityLevel)
            {
                insertIndex = i;
                break;
            }
        }
    }

    for (int i = oldSize; i > insertIndex; i--)
    {
        waitingPatientIds.set(i, waitingPatientIds.get(i - 1));
    }

    waitingPatientIds.set(insertIndex, patientId);
}

void HospitalSim::removeWaitingAt(int index)
{
    int lastIndex = waitingPatientIds.size() - 1;
    if (index < 0 || index > lastIndex)
    {
        return;
    }

    for (int i = index; i < lastIndex; i++)
    {
        waitingPatientIds.set(i, waitingPatientIds.get(i + 1));
    }

    waitingPatientIds.popBack();
}

void HospitalSim::sortWaitingByPriority()
{
    for (int i = 1; i < waitingPatientIds.size(); i++)
    {
        int patientId = waitingPatientIds.get(i);
        Entity *patient = engine->getEntity(patientId);
        if (patient == nullptr)
        {
            continue;
        }

        int patientPriority = patient->priorityLevel;
        int j = i - 1;

        while (j >= 0)
        {
            int queuedId = waitingPatientIds.get(j);
            Entity *queuedPatient = engine->getEntity(queuedId);
            if (queuedPatient == nullptr)
            {
                break;
            }

            if (queuedPatient->priorityLevel <= patientPriority)
            {
                break;
            }

            waitingPatientIds.set(j + 1, queuedId);
            j--;
        }

        waitingPatientIds.set(j + 1, patientId);
    }
}

int HospitalSim::getRandomPriority() const
{
    int roll = std::rand() % 100;
    if (roll < 40)
    {
        return 1;
    }
    if (roll < 75)
    {
        return 3;
    }
    return 5;
}

double HospitalSim::getRandomArrivalGap() const
{
    return arrivalGap;
}

double HospitalSim::getRandomServiceDuration() const
{
    int randomValue = 20 + (std::rand() % 41);
    return (double)randomValue;
}