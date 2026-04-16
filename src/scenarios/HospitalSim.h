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
    int doctorSlotPatient[3][6];
    int doctorSlotReservedPatient[3][6];
    int doctorSlotPriority[3][6];
    int doctorSlotSeen[3][6];
    int doctorBoxCount;
    int doctorBoxRole[6];
    int doctorBoxSlot[6];
    char doctorBoxNames[6][32];
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
    char logLines[500][200];
    int logTypes[500];
    int logCount;
    int eventLogDelay;
    int peakWaitingRoom;
    int countCritical;
    int countUrgent;
    int countNormal;
    int patientsOnTime;

    int doctorIndex(const std::string &type) const;
    const char *roleNameByIndex(int roleIndex) const;
    int preferredRoleByPriority(int priority) const;
    int fallbackRoleByPriority(int priority) const;
    int findFreeDoctorSlot(int roleIndex) const;
    int findReservedDoctorSlotByPatient(int roleIndex, int patientId) const;
    int findDoctorSlotByPatient(int roleIndex, int patientId) const;
    Patient *findFirstWaiting(LinkedList<Patient *> &queue) const;
    void dispatchWaitingPatients();
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
    int getDoctorSeenToday(int doctorIndex) const;
    int getDoctorCurrentPatient(int doctorIndex) const;
    int getDoctorCurrentPriority(int doctorIndex);
    const char *getDoctorName(int doctorIndex) const;
    int getDoctorBoxCount() const;
    const char *getDoctorBoxNameByIndex(int boxIndex) const;
    const char *getDoctorBoxRoleByIndex(int boxIndex) const;
    int getDoctorBoxPatientByIndex(int boxIndex) const;
    int getDoctorBoxPriorityByIndex(int boxIndex) const;
    int getDoctorBoxSeenByIndex(int boxIndex) const;
    int getTotalWaiting() const;
    int getPeakWaitingRoom() const;
    int getCountCritical() const;
    int getCountUrgent() const;
    int getCountNormal() const;
    int getPatientsOnTime() const;
    int getWaitingByPriority(int priority) const;
    int getWaitingRows(int ids[], int priorities[], int waits[], int maxItems) const;
    int getWaitingSnapshot(int ids[], int priorities[], int maxItems, int offset) const;
    int getEventLogCount() const;
    const char *getEventLogLine(int index) const;
    int getEventLogType(int index) const;
    double getCurrentSimTime() const;
};

#endif