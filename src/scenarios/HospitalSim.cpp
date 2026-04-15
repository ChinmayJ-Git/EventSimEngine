#include "HospitalSim.h"

#include "../gui/Renderer.h"
#include "../gui/Window.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

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
    doctors[0].specialization = "Emergency";
    doctors[1].id = 2;
    doctors[1].specialization = "General";
    doctors[2].id = 3;
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
    eventLogCount = 0;
    for (int i = 0; i < 12; i++)
    {
        eventLogLines[i][0] = '\0';
        eventLogTypes[i] = 0;
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
    for (int i = 11; i > 0; i--)
    {
        std::snprintf(eventLogLines[i], 160, "%s", eventLogLines[i - 1]);
        eventLogTypes[i] = eventLogTypes[i - 1];
    }

    std::snprintf(eventLogLines[0], 160, "[t=%05.2f] %s - %s", timeValue, eventName, details);
    eventLogTypes[0] = logType;
    if (eventLogCount < 12)
    {
        eventLogCount++;
    }
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

    Doctor *match = getFreeDoctor(doctorForPriority(p->priority));

    char detail[120];
    if (match != 0)
    {
        std::snprintf(detail, 120, "Patient #%d | Priority: %s | Assigned to: %s Doctor",
                      p->id, priorityLabel(p->priority), match->specialization.c_str());
    }
    else
    {
        std::snprintf(detail, 120, "Patient #%d | Priority: %s | Queue: waiting...",
                      p->id, priorityLabel(p->priority));
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
    std::snprintf(detail, 120, "Patient #%d | Wait was: %.2fs | %s Doctor now FREE",
                  p->id, wait, d->specialization.c_str());
    addEventLog(p->departureTime, "SERVICE END", detail, 3);
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
        updateLiveView();
        return;
    }

    if (e->type == SERVICE_START)
    {
        char detail[120];
        std::snprintf(detail, 120, "%s Doctor - treating Patient #%d",
                      e->doctorType.c_str(), e->patientId);
        addEventLog(e->time, "SERVICE", detail, 1);
        updateLiveView();
        return;
    }

    if (e->type == SERVICE_END)
    {
        int idx = doctorIndex(e->doctorType);
        if (idx >= 0)
        {
            onServiceEnd(engine.getPatient(e->patientId), &doctors[idx]);
        }
        updateLiveView();
        return;
    }

    if (e->type == ESCALATION)
    {
        Patient *p = engine.getPatient(e->patientId);
        if (p == 0 || p->state != WAITING)
        {
            return;
        }

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
            std::snprintf(detail, 120, "Patient #%d | %s -> %s | Wait: %.2fs",
                          p->id, priorityLabel(before), priorityLabel(p->priority), wait);
            addEventLog(e->time, "ESCALATION", detail, 2);
        }
        updateLiveView();
        return;
    }
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
        }
        else if (roll < 60)
        {
            p->priority = 2;
        }
        else
        {
            p->priority = 3;
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

int HospitalSim::getDoctorCurrentPatient(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return -1;
    }
    return doctorCurrentPatient[doctorIndex];
}

const char *HospitalSim::getDoctorLabel(int doctorIndex) const
{
    if (doctorIndex == 0)
    {
        return "EMERGENCY";
    }
    if (doctorIndex == 1)
    {
        return "GENERAL";
    }
    if (doctorIndex == 2)
    {
        return "SPECIALIST";
    }
    return "UNKNOWN";
}

int HospitalSim::getTotalWaiting() const
{
    return criticalQueue.size() + urgentQueue.size() + normalQueue.size();
}

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

int HospitalSim::getEventLogCount() const { return eventLogCount; }

const char *HospitalSim::getEventLogLine(int index) const
{
    if (index < 0 || index >= eventLogCount)
    {
        return "";
    }
    return eventLogLines[index];
}

int HospitalSim::getEventLogType(int index) const
{
    if (index < 0 || index >= eventLogCount)
    {
        return 0;
    }
    return eventLogTypes[index];
}

double HospitalSim::getCurrentSimTime() const
{
    return engine.getCurrentTime();
}
