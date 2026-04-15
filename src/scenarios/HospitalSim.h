#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "../engine/SimEngine.h"
#include "../engine/EscalationManager.h"
#include "../core/LinkedList.h"

#include <string>

class Window;
class Renderer;

class HospitalSim : public EventHandler
{
private:
    SimEngine engine;
    Doctor doctors[3];
    int doctorCapacity[3];
    int doctorBusyNow[3];
    double doctorBusyTime[3];
    int doctorCurrentPatient[3];
    LinkedList<Patient *> criticalQueue;
    LinkedList<Patient *> urgentQueue;
    LinkedList<Patient *> normalQueue;
    int durationSeconds;
    double arrivalRate;
    bool fastMode;
    Window *liveWindow;
    Renderer *liveRenderer;
    int totalPatients;
    double totalWaitTime;
    double longestWait;
    int escalationCount;
    char eventLogLines[12][160];
    int eventLogTypes[12];
    int eventLogCount;

    int doctorIndex(const std::string &type) const;
    void enqueueByPriority(Patient *p);
    void removeFromPriorityQueue(Patient *p, int oldPriority);
    void addEventLog(double timeValue, const char *eventName, const char *details, int logType);
    void updateLiveView();

public:
    HospitalSim(int durationSeconds, double arrivalRate,
                int criticalDoctors = 2, int generalDoctors = 2, int specialistDoctors = 2);
    void attachLiveView(Window *window, Renderer *renderer);
    void setFastMode(bool fastMode);
    void run();
    void onArrival(Patient *p);
    void onServiceEnd(Patient *p, Doctor *d);
    void assignDoctor(Patient *p);
    Doctor *getFreeDoctor(std::string type);
    void onEvent(Event *e);
    void printStats();
    int getTotalPatients() const;
    double getAverageWait() const;
    double getLongestWait() const;
    int getEscalationCount() const;
    double getDoctorUtilisation(int doctorIndex) const;
    int getDoctorCapacity(int doctorIndex) const;
    int getDoctorBusyNow(int doctorIndex) const;
    int getDoctorCurrentPatient(int doctorIndex) const;
    const char *getDoctorLabel(int doctorIndex) const;
    int getTotalWaiting() const;
    int getWaitingByPriority(int priority) const;
    int getWaitingSnapshot(int ids[], int priorities[], int maxItems, int offset) const;
    int getEventLogCount() const;
    const char *getEventLogLine(int index) const;
    int getEventLogType(int index) const;
    double getCurrentSimTime() const;
};

#endif