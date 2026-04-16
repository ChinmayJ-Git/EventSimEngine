#include "HospitalSim.h"

#include "../gui/Renderer.h"
#include "../gui/Window.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <SFML/System.hpp>

static bool isSeeded = false;

static const char *priorityLabel(int priority)
{
    if (priority == 1)
    {
        return "CRITICAL";
    }
    if (priority == 2)
    {
        return "URGENT";
    }
    return "NORMAL";
}

static const char *doctorForPriority(int priority)
{
    if (priority == 1)
    {
        return "Emergency";
    }
    if (priority == 2)
    {
        return "General";
    }
    return "Specialist";
}

HospitalSim::HospitalSim(int durationSeconds, double arrivalRate,
                         int criticalDoctors, int generalDoctors, int specialistDoctors)
    : engine((double)durationSeconds)
{
    this->durationSeconds = durationSeconds;
    this->arrivalRate = arrivalRate;
    if (this->arrivalRate <= 0.0)
    {
        this->arrivalRate = 0.3;
    }
    fastMode = false;
    liveWindow = 0;
    liveRenderer = 0;

    doctors[0].id = 1;
    doctors[0].name = "Dr. Anjali";
    doctors[0].specialization = "Emergency";
    doctors[1].id = 2;
    doctors[1].name = "Dr. Mahesh";
    doctors[1].specialization = "General";
    doctors[2].id = 3;
    doctors[2].name = "Dr. Rahul";
    doctors[2].specialization = "Specialist";

    doctorCapacity[0] = (criticalDoctors < 1) ? 1 : criticalDoctors;
    doctorCapacity[1] = (generalDoctors < 1) ? 1 : generalDoctors;
    doctorCapacity[2] = (specialistDoctors < 1) ? 1 : specialistDoctors;

    for (int i = 0; i < 3; i++)
    {
        doctors[i].isAvailable = true;
        doctors[i].totalPatientsSeen = 0;
        doctorBusyNow[i] = 0;
        doctorBusyTime[i] = 0.0;
        doctorCurrentPatient[i] = -1;
    }

    totalPatients = 0;
    totalWaitTime = 0.0;
    longestWait = 0.0;
    escalationCount = 0;
    logCount = 0;
    eventLogDelay = 0;
    peakWaitingRoom = 0;
    countCritical = 0;
    countUrgent = 0;
    countNormal = 0;
    patientsOnTime = 0;
    for (int i = 0; i < 500; i++)
    {
        logLines[i][0] = '\0';
        logTypes[i] = 0;
    }

    engine.setEventHandler(this);
}

void HospitalSim::attachLiveView(Window *window, Renderer *renderer)
{
    liveWindow = window;
    liveRenderer = renderer;
}

void HospitalSim::setFastMode(bool fastMode)
{
    this->fastMode = fastMode;
}

int HospitalSim::doctorIndex(const std::string &type) const
{
    if (type == "Emergency")
    {
        return 0;
    }
    if (type == "General")
    {
        return 1;
    }
    if (type == "Specialist")
    {
        return 2;
    }
    return -1;
}

void HospitalSim::enqueueByPriority(Patient *p)
{
    if (p == 0)
    {
        return;
    }
    if (p->priority == 1)
    {
        criticalQueue.addBack(p);
    }
    else if (p->priority == 2)
    {
        urgentQueue.addBack(p);
    }
    else
    {
        normalQueue.addBack(p);
    }
}

void HospitalSim::removeFromPriorityQueue(Patient *p, int oldPriority)
{
    if (p == 0)
    {
        return;
    }
    if (oldPriority == 1)
    {
        criticalQueue.removeItem(p);
    }
    else if (oldPriority == 2)
    {
        urgentQueue.removeItem(p);
    }
    else
    {
        normalQueue.removeItem(p);
    }
}

void HospitalSim::addEventLog(double timeValue, const char *eventName, const char *details, int logType)
{
    if (logCount >= 500)
    {
        for (int i = 1; i < 500; i++)
        {
            std::snprintf(logLines[i - 1], 200, "%s", logLines[i]);
            logTypes[i - 1] = logTypes[i];
        }
        logCount = 499;
    }

    std::snprintf(logLines[logCount], 200, "[t=%.1fs]  %-10s >> %s", timeValue, eventName, details);
    logTypes[logCount] = logType;
    logCount++;
}

void HospitalSim::updateLiveView()
{
    if (liveWindow == 0 || liveRenderer == 0)
    {
        return;
    }
    if (!liveWindow->checkOpen())
    {
        return;
    }
    liveWindow->pollEvents();
    liveWindow->clear();
    liveRenderer->drawAll(this);
    liveWindow->display();
}

Doctor *HospitalSim::getFreeDoctor(std::string type)
{
    int idx = doctorIndex(type);
    if (idx < 0)
    {
        return 0;
    }
    doctors[idx].isAvailable = (doctorBusyNow[idx] < doctorCapacity[idx]);
    if (doctors[idx].isAvailable)
    {
        return &doctors[idx];
    }
    return 0;
}

void HospitalSim::onArrival(Patient *p)
{
    if (p == 0)
    {
        return;
    }
    p->state = WAITING;
    enqueueByPriority(p);

    int waitingNow = getTotalWaiting();
    if (waitingNow > peakWaitingRoom)
    {
        peakWaitingRoom = waitingNow;
    }

    Doctor *match = getFreeDoctor(doctorForPriority(p->priority));

    char detail[120];
    if (match != 0)
    {
        std::snprintf(detail, 120, "Patient #%d (%s) arrived, assigned to %s Doctor",
                      p->id, priorityLabel(p->priority), match->specialization.c_str());
    }
    else
    {
        std::snprintf(detail, 120, "Patient #%d (%s) arrived, waiting for %s Doctor",
                      p->id, priorityLabel(p->priority), doctorForPriority(p->priority));
    }
    addEventLog(p->arrivalTime, "ARRIVAL", detail, 0);

    EscalationManager::scheduleEscalation(p, &engine);
    assignDoctor(p);
}

void HospitalSim::assignDoctor(Patient *p)
{
    if (p == 0 || p->state != WAITING)
    {
        return;
    }
    std::string doctorType = doctorForPriority(p->priority);
    Doctor *d = getFreeDoctor(doctorType);
    if (d == 0)
    {
        return;
    }

    removeFromPriorityQueue(p, p->priority);
    int idx = doctorIndex(doctorType);
    if (idx < 0)
    {
        return;
    }
    doctorBusyNow[idx]++;
    d->isAvailable = (doctorBusyNow[idx] < doctorCapacity[idx]);
    doctorCurrentPatient[idx] = p->id;

    p->assignedDoctorType = doctorType;
    p->serviceStartTime = engine.getCurrentTime();
    if (p->serviceStartTime < p->arrivalTime)
    {
        p->serviceStartTime = p->arrivalTime;
    }
    p->state = WAITING;

    engine.scheduleEvent(SERVICE_START, p->serviceStartTime, p->id, doctorType);
    p->departureTime = p->serviceStartTime + (double)(20 + std::rand() % 41);
    engine.scheduleEvent(SERVICE_END, p->departureTime, p->id, doctorType);
}

void HospitalSim::onServiceEnd(Patient *p, Doctor *d)
{
    if (p == 0 || d == 0)
    {
        return;
    }
    int idx = doctorIndex(d->specialization);
    if (idx < 0)
    {
        return;
    }

    if (doctorBusyNow[idx] > 0)
    {
        doctorBusyNow[idx]--;
    }
    d->isAvailable = (doctorBusyNow[idx] < doctorCapacity[idx]);
    if (doctorBusyNow[idx] == 0)
    {
        doctorCurrentPatient[idx] = -1;
    }
    d->totalPatientsSeen++;

    double busy = p->departureTime - p->serviceStartTime;
    if (busy < 0.0)
    {
        busy = 0.0;
    }
    doctorBusyTime[idx] += busy;

    double wait = p->serviceStartTime - p->arrivalTime;
    if (wait < 0.0)
    {
        wait = 0.0;
    }
    totalWaitTime += wait;
    if (wait > longestWait)
    {
        longestWait = wait;
    }
    if (wait < 30.0)
    {
        patientsOnTime++;
    }

    if (idx == 0 && !criticalQueue.isEmpty())
    {
        assignDoctor(criticalQueue.peekFront());
    }
    if (idx == 1 && !urgentQueue.isEmpty())
    {
        assignDoctor(urgentQueue.peekFront());
    }
    if (idx == 2 && !normalQueue.isEmpty())
    {
        assignDoctor(normalQueue.peekFront());
    }

    char detail[120];
    std::snprintf(detail, 120, "Patient #%d (%s) discharged | Wait: %.1fs | Dr. %s free",
                  p->id, priorityLabel(p->priority), wait, d->specialization.c_str());
    addEventLog(p->departureTime, "DISCHARGE", detail, 3);
}

void HospitalSim::onEvent(Event *e)
{
    if (e == 0)
    {
        return;
    }

    if (e->type == ARRIVAL)
    {
        onArrival(engine.getPatient(e->patientId));
    }
    else if (e->type == SERVICE_START)
    {
        Patient *p = engine.getPatient(e->patientId);
        const char *priority = (p == 0) ? "UNKNOWN" : priorityLabel(p->priority);
        char detail[120];
        std::snprintf(detail, 120, "Dr. %s started treating Patient #%d (%s)",
                      e->doctorType.c_str(), e->patientId, priority);
        addEventLog(e->time, "TREATMENT", detail, 1);
    }
    else if (e->type == SERVICE_END)
    {
        int idx = doctorIndex(e->doctorType);
        if (idx >= 0)
        {
            onServiceEnd(engine.getPatient(e->patientId), &doctors[idx]);
        }
    }
    else if (e->type == ESCALATION)
    {
        Patient *p = engine.getPatient(e->patientId);
        if (p != 0 && p->state == WAITING)
        {
            int before = p->priority;
            EscalationManager::processEscalation(p, &engine);
            if (p->priority < before)
            {
                escalationCount++;
                double wait = e->time - p->arrivalTime;
                if (wait < 0.0)
                {
                    wait = 0.0;
                }
                removeFromPriorityQueue(p, before);
                enqueueByPriority(p);
                assignDoctor(p);

                char detail[120];
                std::snprintf(detail, 120, "Patient #%d upgraded %s -> %s after waiting %ds",
                              p->id, priorityLabel(before), priorityLabel(p->priority), (int)(wait + 0.5));
                addEventLog(e->time, "ESCALATION", detail, 2);
            }
        }
    }

    updateLiveView();
    eventLogDelay += 5000;
    int pauseMs = fastMode ? 4900 : 4500;
    if (pauseMs < 0)
    {
        pauseMs = 0;
    }
    sf::sleep(sf::milliseconds(pauseMs));
}

void HospitalSim::run()
{
    if (!isSeeded)
    {
        std::srand((unsigned int)std::time(0));
        isSeeded = true;
    }

    double t = 0.0;
    int idCounter = 1;

    while (t < (double)durationSeconds)
    {
        t += (1.0 + (double)(std::rand() % 100) / 100.0) / arrivalRate;
        if (t > (double)durationSeconds)
        {
            break;
        }

        Patient *p = new Patient();
        p->id = idCounter;
        idCounter++;
        p->arrivalTime = t;

        int roll = std::rand() % 100;
        if (roll < 25)
        {
            p->priority = 1;
            countCritical++;
        }
        else if (roll < 60)
        {
            p->priority = 2;
            countUrgent++;
        }
        else
        {
            p->priority = 3;
            countNormal++;
        }

        engine.addPatient(p);
        totalPatients++;
        engine.scheduleEvent(ARRIVAL, t, p->id, "");
    }

    engine.setFastMode(fastMode);
    engine.run();
}

void HospitalSim::printStats()
{
    // Stats are shown in the GUI and exported to CSV.
}

int HospitalSim::getTotalPatients() const { return totalPatients; }

double HospitalSim::getAverageWait() const
{
    if (totalPatients == 0)
    {
        return 0.0;
    }
    return totalWaitTime / (double)totalPatients;
}

double HospitalSim::getLongestWait() const { return longestWait; }

int HospitalSim::getEscalationCount() const { return escalationCount; }

double HospitalSim::getDoctorUtilisation(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2 || durationSeconds <= 0)
    {
        return 0.0;
    }
    double capacityTime = (double)durationSeconds * (double)doctorCapacity[doctorIndex];
    if (capacityTime <= 0.0)
    {
        return 0.0;
    }
    return (doctorBusyTime[doctorIndex] / capacityTime) * 100.0;
}

int HospitalSim::getDoctorCapacity(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return 0;
    }
    return doctorCapacity[doctorIndex];
}

int HospitalSim::getDoctorBusyNow(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return 0;
    }
    return doctorBusyNow[doctorIndex];
}

int HospitalSim::getDoctorSeenToday(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return 0;
    }
    return doctors[doctorIndex].totalPatientsSeen;
}

int HospitalSim::getDoctorCurrentPatient(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return -1;
    }
    return doctorCurrentPatient[doctorIndex];
}

int HospitalSim::getDoctorCurrentPriority(int doctorIndex)
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return 0;
    }
    int patientId = doctorCurrentPatient[doctorIndex];
    if (patientId < 0)
    {
        return 0;
    }
    Patient *p = engine.getPatient(patientId);
    if (p == 0)
    {
        return 0;
    }
    return p->priority;
}

const char *HospitalSim::getDoctorName(int doctorIndex) const
{
    if (doctorIndex == 0)
    {
        return doctors[0].name.c_str();
    }
    if (doctorIndex == 1)
    {
        return doctors[1].name.c_str();
    }
    if (doctorIndex == 2)
    {
        return doctors[2].name.c_str();
    }
    return "UNKNOWN";
}

int HospitalSim::getTotalWaiting() const
{
    return criticalQueue.size() + urgentQueue.size() + normalQueue.size();
}

int HospitalSim::getPeakWaitingRoom() const { return peakWaitingRoom; }

int HospitalSim::getCountCritical() const { return countCritical; }

int HospitalSim::getCountUrgent() const { return countUrgent; }

int HospitalSim::getCountNormal() const { return countNormal; }

int HospitalSim::getPatientsOnTime() const { return patientsOnTime; }

int HospitalSim::getWaitingByPriority(int priority) const
{
    if (priority == 1)
    {
        return criticalQueue.size();
    }
    if (priority == 2)
    {
        return urgentQueue.size();
    }
    return normalQueue.size();
}

int HospitalSim::getWaitingRows(int ids[], int priorities[], int waits[], int maxItems) const
{
    if (ids == 0 || priorities == 0 || waits == 0 || maxItems <= 0)
    {
        return 0;
    }

    int written = 0;
    double now = engine.getCurrentTime();

    LinkedList<Patient *>::Node *node = criticalQueue.getHead();
    while (node != 0 && written < maxItems)
    {
        ids[written] = node->data->id;
        priorities[written] = 1;
        double wait = now - node->data->arrivalTime;
        if (wait < 0.0)
        {
            wait = 0.0;
        }
        waits[written] = (int)(wait + 0.5);
        written++;
        node = node->next;
    }

    node = urgentQueue.getHead();
    while (node != 0 && written < maxItems)
    {
        ids[written] = node->data->id;
        priorities[written] = 2;
        double wait = now - node->data->arrivalTime;
        if (wait < 0.0)
        {
            wait = 0.0;
        }
        waits[written] = (int)(wait + 0.5);
        written++;
        node = node->next;
    }

    node = normalQueue.getHead();
    while (node != 0 && written < maxItems)
    {
        ids[written] = node->data->id;
        priorities[written] = 3;
        double wait = now - node->data->arrivalTime;
        if (wait < 0.0)
        {
            wait = 0.0;
        }
        waits[written] = (int)(wait + 0.5);
        written++;
        node = node->next;
    }

    return written;
}

int HospitalSim::getWaitingSnapshot(int ids[], int priorities[], int maxItems, int offset) const
{
    if (ids == 0 || priorities == 0 || maxItems <= 0)
    {
        return 0;
    }

    int skipped = 0;
    int written = 0;

    LinkedList<Patient *>::Node *node = criticalQueue.getHead();
    while (node != 0 && written < maxItems)
    {
        if (skipped < offset)
        {
            skipped++;
        }
        else
        {
            ids[written] = node->data->id;
            priorities[written] = 1;
            written++;
        }
        node = node->next;
    }

    node = urgentQueue.getHead();
    while (node != 0 && written < maxItems)
    {
        if (skipped < offset)
        {
            skipped++;
        }
        else
        {
            ids[written] = node->data->id;
            priorities[written] = 2;
            written++;
        }
        node = node->next;
    }

    node = normalQueue.getHead();
    while (node != 0 && written < maxItems)
    {
        if (skipped < offset)
        {
            skipped++;
        }
        else
        {
            ids[written] = node->data->id;
            priorities[written] = 3;
            written++;
        }
        node = node->next;
    }

    return written;
}

int HospitalSim::getEventLogCount() const { return logCount; }

const char *HospitalSim::getEventLogLine(int index) const
{
    if (index < 0 || index >= logCount)
    {
        return "";
    }
    return logLines[index];
}

int HospitalSim::getEventLogType(int index) const
{
    if (index < 0 || index >= logCount)
    {
        return 0;
    }
    return logTypes[index];
}

double HospitalSim::getCurrentSimTime() const
{
    return engine.getCurrentTime();
}
