#ifndef HOSPITALSIM_H
#define HOSPITALSIM_H

#include "../engine/Event.h"
#include "../engine/Entity.h"

// hospital simulation
class HospitalSim {

private:
    int totalDoctors;
    int availableDoctors;

    int totalBeds;
    int availableBeds;

    double escalationTimeLimit;

public:

    // constructor
    HospitalSim(int doctors, int beds, double escalationLimit);

<<<<<<< HEAD
    // process event
    void processEvent(Event event);

    // arrival
=======
    // handle event
    void processEvent(Event event);

    // patient arrival
>>>>>>> eb00a90e5812d905f775f5d5a5369cc59acbe24a
    void handleArrival(Entity* patient, double currentTime);

    // start treatment
    void handleTreatmentStart(Entity* patient, double currentTime);

    // end treatment
    void handleTreatmentEnd(Entity* patient, double currentTime);

<<<<<<< HEAD
    // escalation
=======
    // escalation event
>>>>>>> eb00a90e5812d905f775f5d5a5369cc59acbe24a
    void handleEscalation(Entity* patient, double currentTime);

};

#endif
