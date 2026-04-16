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
        return "SPECIFIC";
    }
    return "NORMAL";
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

    int c = criticalDoctors;
    int g = generalDoctors;
    int s = specialistDoctors;
    if (c < 1)
    {
        c = 1;
    }
    if (g < 1)
    {
        g = 1;
    }
    if (s < 1)
    {
        s = 1;
    }
    if (c > 6)
    {
        c = 6;
    }
    if (g > 6)
    {
        g = 6;
    }
    if (s > 6)
    {
        s = 6;
    }
    while (c + g + s > 6)
    {
        if (c >= g && c >= s && c > 1)
        {
            c--;
        }
        else if (g >= s && g > 1)
        {
            g--;
        }
        else if (s > 1)
        {
            s--;
        }
        else
        {
            break;
        }
    }

    doctorCapacity[0] = c;
    doctorCapacity[1] = g;
    doctorCapacity[2] = s;

    for (int i = 0; i < 3; i++)
    {
        doctors[i].isAvailable = true;
        doctors[i].totalPatientsSeen = 0;
        doctorBusyNow[i] = 0;
        doctorBusyTime[i] = 0.0;
        doctorCurrentPatient[i] = -1;
        for (int slot = 0; slot < 6; slot++)
        {
            doctorSlotPatient[i][slot] = -1;
            doctorSlotReservedPatient[i][slot] = -1;
            doctorSlotPriority[i][slot] = 0;
            doctorSlotSeen[i][slot] = 0;
        }
    }

    const char *namePool[7] = {
        "Dr. Anjali", "Dr. Mahesh", "Dr. Rahul", "Dr. Debasis",
        "Dr. Shanmukha", "Dr. Ankit", "Dr. Ram"};
    doctorBoxCount = 0;
    int nextName = 0;
    for (int role = 0; role < 3; role++)
    {
        for (int slot = 0; slot < doctorCapacity[role] && doctorBoxCount < 6; slot++)
        {
            doctorBoxRole[doctorBoxCount] = role;
            doctorBoxSlot[doctorBoxCount] = slot;
            std::snprintf(doctorBoxNames[doctorBoxCount], 32, "%s", namePool[nextName % 7]);
            doctorBoxCount++;
            nextName++;
        }
    }

    for (int role = 0; role < 3; role++)
    {
        for (int i = 0; i < doctorBoxCount; i++)
        {
            if (doctorBoxRole[i] == role)
            {
                doctors[role].name = doctorBoxNames[i];
                break;
            }
        }
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

const char *HospitalSim::roleNameByIndex(int roleIndex) const
{
    if (roleIndex == 0)
    {
        return "Emergency";
    }
    if (roleIndex == 1)
    {
        return "General";
    }
    return "Specialist";
}

int HospitalSim::preferredRoleByPriority(int priority) const
{
    if (priority == 1)
    {
        return 0;
    }
    if (priority == 2)
    {
        return 2;
    }
    return 1;
}

int HospitalSim::fallbackRoleByPriority(int priority) const
{
    if (priority == 1)
    {
        return 1;
    }
    return -1;
}

int HospitalSim::findFreeDoctorSlot(int roleIndex) const
{
    if (roleIndex < 0 || roleIndex > 2)
    {
        return -1;
    }
    for (int slot = 0; slot < doctorCapacity[roleIndex]; slot++)
    {
        if (doctorSlotPatient[roleIndex][slot] < 0 && doctorSlotReservedPatient[roleIndex][slot] < 0)
        {
            return slot;
        }
    }
    return -1;
}

int HospitalSim::findReservedDoctorSlotByPatient(int roleIndex, int patientId) const
{
    if (roleIndex < 0 || roleIndex > 2)
    {
        return -1;
    }
    for (int slot = 0; slot < doctorCapacity[roleIndex]; slot++)
    {
        if (doctorSlotReservedPatient[roleIndex][slot] == patientId)
        {
            return slot;
        }
    }
    return -1;
}

int HospitalSim::findDoctorSlotByPatient(int roleIndex, int patientId) const
{
    if (roleIndex < 0 || roleIndex > 2)
    {
        return -1;
    }
    for (int slot = 0; slot < doctorCapacity[roleIndex]; slot++)
    {
        if (doctorSlotPatient[roleIndex][slot] == patientId)
        {
            return slot;
        }
    }
    return -1;
}

Patient *HospitalSim::findFirstWaiting(LinkedList<Patient *> &queue) const
{
    LinkedList<Patient *>::Node *node = queue.getHead();
    while (node != 0)
    {
        if (node->data != 0 && node->data->state == WAITING)
        {
            return node->data;
        }
        node = node->next;
    }
    return 0;
}

void HospitalSim::dispatchWaitingPatients()
{
    bool assignedAny = true;
    while (assignedAny)
    {
        assignedAny = false;

        int beforeAssigned = doctorBusyNow[0] + doctorBusyNow[1] + doctorBusyNow[2];

        Patient *critical = findFirstWaiting(criticalQueue);
        if (critical != 0)
        {
            assignDoctor(critical);
        }

        Patient *specific = findFirstWaiting(urgentQueue);
        if (specific != 0)
        {
            assignDoctor(specific);
        }

        Patient *normal = findFirstWaiting(normalQueue);
        if (normal != 0)
        {
            assignDoctor(normal);
        }

        int afterAssigned = doctorBusyNow[0] + doctorBusyNow[1] + doctorBusyNow[2];
        if (afterAssigned > beforeAssigned)
        {
            assignedAny = true;
        }
    }
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

    std::snprintf(logLines[logCount], 200, "[t=%.1fs] %-10s >> %s", timeValue, eventName, details);
    logTypes[logCount] = logType;
    logCount++;

    sf::Clock logClock;
    while (logClock.getElapsedTime().asMilliseconds() < 5000)
    {
        updateLiveView();
        if (liveWindow != 0 && !liveWindow->checkOpen())
        {
            break;
        }
        sf::sleep(sf::milliseconds(30));
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
    p->serviceStartTime = -1.0;
    p->departureTime = -1.0;
    p->assignedDoctorType = "";
    enqueueByPriority(p);

    int waitingNow = getTotalWaiting();
    if (waitingNow > peakWaitingRoom)
    {
        peakWaitingRoom = waitingNow;
    }

    char detail[120];
    std::snprintf(detail, 120, "Patient #%d (%s) arrived and entered waiting area",
                  p->id, priorityLabel(p->priority));
    addEventLog(p->arrivalTime, "ARRIVAL", detail, 0);

    EscalationManager::scheduleEscalation(p, &engine);
    dispatchWaitingPatients();
}

void HospitalSim::assignDoctor(Patient *p)
{
    if (p == 0 || p->state != WAITING)
    {
        return;
    }

    int roleIndex = -1;
    int slotIndex = -1;
    if (p->priority == 1)
    {
        roleIndex = 0;
        slotIndex = findFreeDoctorSlot(roleIndex);
        if (slotIndex < 0)
        {
            double waited = engine.getCurrentTime() - p->arrivalTime;
            if (waited >= 60.0)
            {
                roleIndex = 1;
                slotIndex = findFreeDoctorSlot(roleIndex);
            }
        }
    }
    else if (p->priority == 2)
    {
        roleIndex = 2;
        slotIndex = findFreeDoctorSlot(roleIndex);
    }
    else
    {
        roleIndex = 1;
        slotIndex = findFreeDoctorSlot(roleIndex);
    }

    if (slotIndex < 0)
    {
        return;
    }

    doctorBusyNow[roleIndex]++;
    doctors[roleIndex].isAvailable = (doctorBusyNow[roleIndex] < doctorCapacity[roleIndex]);
    doctorSlotReservedPatient[roleIndex][slotIndex] = p->id;

    p->assignedDoctorType = roleNameByIndex(roleIndex);
    double baseStart = engine.getCurrentTime();
    if (baseStart < p->arrivalTime)
    {
        baseStart = p->arrivalTime;
    }
    double triageDelay = (double)(3 + std::rand() % 6);
    p->serviceStartTime = baseStart + triageDelay;
    p->state = IDLE;
    p->escalationDeadline = -1.0;

    engine.scheduleEvent(SERVICE_START, p->serviceStartTime, p->id, p->assignedDoctorType);
    p->departureTime = p->serviceStartTime + (double)(22 + std::rand() % 29);
    engine.scheduleEvent(SERVICE_END, p->departureTime, p->id, p->assignedDoctorType);
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
    int slot = findDoctorSlotByPatient(idx, p->id);
    if (slot >= 0)
    {
        doctorSlotPatient[idx][slot] = -1;
        doctorSlotPriority[idx][slot] = 0;
        doctorSlotSeen[idx][slot]++;
    }

    d->isAvailable = (doctorBusyNow[idx] < doctorCapacity[idx]);
    doctorCurrentPatient[idx] = -1;
    for (int s = 0; s < doctorCapacity[idx]; s++)
    {
        if (doctorSlotPatient[idx][s] >= 0)
        {
            doctorCurrentPatient[idx] = doctorSlotPatient[idx][s];
            break;
        }
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

    dispatchWaitingPatients();

    char detail[120];
    std::snprintf(detail, 120, "Patient #%d (%s) discharged | Wait: %.1fs | %s doctor free",
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
        int idx = doctorIndex(e->doctorType);
        if (p != 0 && idx >= 0)
        {
            int reservedSlot = findReservedDoctorSlotByPatient(idx, p->id);
            if (reservedSlot >= 0)
            {
                doctorSlotReservedPatient[idx][reservedSlot] = -1;
                doctorSlotPatient[idx][reservedSlot] = p->id;
                doctorSlotPriority[idx][reservedSlot] = p->priority;
            }
            removeFromPriorityQueue(p, p->priority);
            doctorCurrentPatient[idx] = p->id;
            doctors[idx].isAvailable = (doctorBusyNow[idx] < doctorCapacity[idx]);
        }
        const char *priority = (p == 0) ? "UNKNOWN" : priorityLabel(p->priority);
        char detail[120];
        std::snprintf(detail, 120, "%s doctor started treating Patient #%d (%s)",
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
                std::snprintf(detail, 120, "Patient #%d escalated %s -> %s after waiting %ds",
                              p->id, priorityLabel(before), priorityLabel(p->priority), (int)(wait + 0.5));
                addEventLog(e->time, "ESCALATION", detail, 2);
            }
        }
    }

    updateLiveView();
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
        double gap = (1.0 + (double)(std::rand() % 100) / 100.0) / arrivalRate;
        if ((std::rand() % 100) < 30)
        {
            gap *= 0.30;
        }
        if (gap < 0.5)
        {
            gap = 0.5;
        }
        t += gap;
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
    double simSeconds = engine.getCurrentTime();
    if (simSeconds < (double)durationSeconds)
    {
        simSeconds = (double)durationSeconds;
    }
    double capacityTime = simSeconds * (double)doctorCapacity[doctorIndex];
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
    for (int slot = 0; slot < doctorCapacity[doctorIndex]; slot++)
    {
        if (doctorSlotPatient[doctorIndex][slot] >= 0)
        {
            return doctorSlotPriority[doctorIndex][slot];
        }
    }
    return 0;
}

const char *HospitalSim::getDoctorName(int doctorIndex) const
{
    if (doctorIndex < 0 || doctorIndex > 2)
    {
        return "UNKNOWN";
    }
    for (int i = 0; i < doctorBoxCount; i++)
    {
        if (doctorBoxRole[i] == doctorIndex)
        {
            return doctorBoxNames[i];
        }
    }
    return "UNKNOWN";
}

int HospitalSim::getDoctorBoxCount() const { return doctorBoxCount; }

const char *HospitalSim::getDoctorBoxNameByIndex(int boxIndex) const
{
    if (boxIndex < 0 || boxIndex >= doctorBoxCount)
    {
        return "UNKNOWN";
    }
    return doctorBoxNames[boxIndex];
}

const char *HospitalSim::getDoctorBoxRoleByIndex(int boxIndex) const
{
    if (boxIndex < 0 || boxIndex >= doctorBoxCount)
    {
        return "Unknown";
    }
    return roleNameByIndex(doctorBoxRole[boxIndex]);
}

int HospitalSim::getDoctorBoxPatientByIndex(int boxIndex) const
{
    if (boxIndex < 0 || boxIndex >= doctorBoxCount)
    {
        return -1;
    }
    int role = doctorBoxRole[boxIndex];
    int slot = doctorBoxSlot[boxIndex];
    return doctorSlotPatient[role][slot];
}

int HospitalSim::getDoctorBoxPriorityByIndex(int boxIndex) const
{
    if (boxIndex < 0 || boxIndex >= doctorBoxCount)
    {
        return 0;
    }
    int role = doctorBoxRole[boxIndex];
    int slot = doctorBoxSlot[boxIndex];
    return doctorSlotPriority[role][slot];
}

int HospitalSim::getDoctorBoxSeenByIndex(int boxIndex) const
{
    if (boxIndex < 0 || boxIndex >= doctorBoxCount)
    {
        return 0;
    }
    int role = doctorBoxRole[boxIndex];
    int slot = doctorBoxSlot[boxIndex];
    return doctorSlotSeen[role][slot];
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
