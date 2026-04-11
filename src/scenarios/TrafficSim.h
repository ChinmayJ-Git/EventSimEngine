#ifndef TRAFFICSIM_H
#define TRAFFICSIM_H

#include "Event.h"

// traffic simulation
class TrafficSim {

private:
    int greenSignal;          // 0 = road A, 1 = road B

    int queueA[100];
    int queueB[100];

    int countA;
    int countB;

public:

    // constructor
    TrafficSim();

    // process event
    void processEvent(Event event);

    // handlers
    void handleArrival(int carId, int road, double currentTime);
    void handleSignalCheck(double currentTime);

};

#endif