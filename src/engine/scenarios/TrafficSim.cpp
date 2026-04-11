#include "TrafficSim.h"
#include <iostream>

// constructor
TrafficSim::TrafficSim() {
    greenSignal = 0; // road A starts green
    countA = 0;
    countB = 0;
}

// process event
void TrafficSim::processEvent(Event event) {

    int carId = event.entityId;
    int road = event.locationId;
    double currentTime = event.time;

    if (event.type == ARRIVAL) {
        handleArrival(carId, road, currentTime);
    }
    else if (event.type == SIGNAL_CHECK) {
        handleSignalCheck(currentTime);
    }
}

// arrival
void TrafficSim::handleArrival(int carId, int road, double currentTime) {

    std::cout << "Car arrived: " << carId << " on road " << road << "\n";

    if (road == 0) {
        queueA[countA++] = carId;
    } else {
        queueB[countB++] = carId;
    }
}

// signal check (adaptive control)
void TrafficSim::handleSignalCheck(double currentTime) {

    std::cout << "Signal check at time: " << currentTime << "\n";

    // choose road with more cars
    if (countA > countB) {
        greenSignal = 0;
    } else {
        greenSignal = 1;
    }

    std::cout << "Green signal for road: " << greenSignal << "\n";

    // allow one car to pass
    if (greenSignal == 0 && countA > 0) {
        std::cout << "Car passed from road A: " << queueA[0] << "\n";

        for (int i = 1; i < countA; i++) {
            queueA[i - 1] = queueA[i];
        }
        countA--;
    }
    else if (greenSignal == 1 && countB > 0) {
        std::cout << "Car passed from road B: " << queueB[0] << "\n";

        for (int i = 1; i < countB; i++) {
            queueB[i - 1] = queueB[i];
        }
        countB--;
    }
}