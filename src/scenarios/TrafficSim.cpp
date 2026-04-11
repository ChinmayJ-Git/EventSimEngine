#include "src/scenarios/TrafficSim.h"
#include "src/scenarios/HospitalSim.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

static bool trafficSeeded = false;

TrafficSim::TrafficSim(SimEngine *engine, int numberOfIntersections,
                       double duration)
{
    this->engine = engine;
    this->numberOfIntersections = numberOfIntersections;
    this->duration = duration;

    nextEntityId = 20000;

    totalCars = 0;
    totalPassed = 0;
    totalCarWaitTime = 0.0;
    longestCarWait = 0.0;
    busiestIntersection = 0;

    if (!trafficSeeded)
    {
        std::srand((unsigned int)std::time(nullptr));
        trafficSeeded = true;
    }
}

TrafficSim::~TrafficSim()
{
    for (int i = 0; i < waitingCarIds.size(); i++)
    {
        delete waitingCarIds.get(i);
    }

    for (int i = 0; i < waitingArrivalTimes.size(); i++)
    {
        delete waitingArrivalTimes.get(i);
    }
}

void TrafficSim::initialise()
{
    buildRoadGraph();

    for (int i = 0; i < numberOfIntersections; i++)
    {
        waitingCarIds.pushBack(new DynamicArray<int>());
        waitingArrivalTimes.pushBack(new DynamicArray<double>());
        carsPassedPerIntersection.pushBack(0);

        lightStates.pushBack(1);
        signalTimers.pushBack(30.0);

        int lightId = nextEntityId;
        nextEntityId++;

        std::string lightName = "Light_" + std::to_string(i);
        Entity *light = new Entity(lightId, TRAFFIC_LIGHT, lightName, 0.0, 1);
        light->state = IDLE;
        light->currentLocationId = i;
        engine->addEntity(light);
        lightEntityIds.pushBack(lightId);

        engine->scheduleEvent(30.0, SIGNAL_CHECK, lightId, i, "signal checks");
        engine->scheduleEvent(30.0, SIGNAL_CHANGE, lightId, i, "signal changes");
    }

    double arrivalTime = 1.0;
    while (arrivalTime <= duration)
    {
        int carId = nextEntityId;
        nextEntityId++;

        int intersectionId = std::rand() % numberOfIntersections;
        std::string carName = "Car_" + std::to_string(carId);

        Entity *car = new Entity(carId, CAR, carName, arrivalTime, 5);
        car->state = MOVING;
        car->currentLocationId = intersectionId;

        engine->addEntity(car);
        totalCars++;

        engine->scheduleEvent(arrivalTime, ARRIVAL, carId,
                              intersectionId, "car arrives");

        arrivalTime = arrivalTime + getRandomCarGap();
    }
}

void TrafficSim::run()
{
    engine->run();
}

void TrafficSim::printResults()
{
    double averageWait = 0.0;
    if (totalPassed > 0)
    {
        averageWait = totalCarWaitTime / totalPassed;
    }

    std::cout << "Traffic results" << std::endl;
    std::cout << "Cars total: " << totalCars << std::endl;
    std::cout << "Cars passed: " << totalPassed << std::endl;
    std::cout << "Average wait: " << averageWait << std::endl;
    std::cout << "Longest wait: " << longestCarWait << std::endl;
    std::cout << "Busiest node: " << busiestIntersection << std::endl;
}

void TrafficSim::buildRoadGraph()
{
    if (numberOfIntersections < 0)
    {
        numberOfIntersections = 0;
    }
}

void TrafficSim::handleArrival(Event *eventData)
{
    int intersectionId = eventData->locationId;
    if (intersectionId < 0 || intersectionId >= numberOfIntersections)
    {
        return;
    }

    Entity *car = engine->getEntity(eventData->entityId);
    if (car == nullptr)
    {
        return;
    }

    car->currentLocationId = intersectionId;

    if (lightStates.get(intersectionId) == 1)
    {
        engine->scheduleEvent(eventData->time + 3.0, DEPARTURE,
                              car->id, intersectionId, "car departs");
    }
    else
    {
        car->state = WAITING;
        waitingCarIds.get(intersectionId)->pushBack(car->id);
        waitingArrivalTimes.get(intersectionId)->pushBack(eventData->time);
        updateBusiestByQueue(intersectionId);
    }
}

void TrafficSim::handleSignalCheck(Event *eventData)
{
    int intersectionId = eventData->locationId;
    if (intersectionId < 0 || intersectionId >= numberOfIntersections)
    {
        return;
    }

    int localCount = waitingCarIds.get(intersectionId)->size();
    bool extendTimer = false;

    if (numberOfIntersections > 1)
    {
        int neighborIds[4];
        neighborIds[0] = (intersectionId + 1) % numberOfIntersections;
        neighborIds[1] = (intersectionId - 1 + numberOfIntersections) % numberOfIntersections;
        neighborIds[2] = (intersectionId + 2) % numberOfIntersections;
        neighborIds[3] = (intersectionId - 2 + numberOfIntersections) % numberOfIntersections;

        for (int i = 0; i < 4; i++)
        {
            int neighborId = neighborIds[i];

            bool seenBefore = false;
            for (int j = 0; j < i; j++)
            {
                if (neighborIds[j] == neighborId)
                {
                    seenBefore = true;
                }
            }

            if (seenBefore)
            {
                continue;
            }

            int neighborCount = waitingCarIds.get(neighborId)->size();
            if (localCount > (neighborCount * 2))
            {
                extendTimer = true;
            }
        }
    }

    if (extendTimer)
    {
        double timer = signalTimers.get(intersectionId);
        signalTimers.set(intersectionId, timer + 10.0);
    }

    engine->scheduleEvent(eventData->time + 30.0, SIGNAL_CHECK,
                          lightEntityIds.get(intersectionId),
                          intersectionId, "signal checks");
}

void TrafficSim::handleSignalChange(Event *eventData)
{
    int intersectionId = eventData->locationId;
    if (intersectionId < 0 || intersectionId >= numberOfIntersections)
    {
        return;
    }

    DynamicArray<int> *queue = waitingCarIds.get(intersectionId);
    DynamicArray<double> *waits = waitingArrivalTimes.get(intersectionId);

    for (int i = 0; i < queue->size(); i++)
    {
        int carId = queue->get(i);
        double waitTime = eventData->time - waits->get(i);
        totalCarWaitTime = totalCarWaitTime + waitTime;
        if (waitTime > longestCarWait)
        {
            longestCarWait = waitTime;
        }

        engine->scheduleEvent(eventData->time + 2.0, DEPARTURE,
                              carId, intersectionId, "car departs");
    }

    queue->clear();
    waits->clear();

    int currentState = lightStates.get(intersectionId);
    if (currentState == 1)
    {
        lightStates.set(intersectionId, 0);
    }
    else
    {
        lightStates.set(intersectionId, 1);
    }

    double nextTimer = signalTimers.get(intersectionId);
    engine->scheduleEvent(eventData->time + nextTimer, SIGNAL_CHANGE,
                          lightEntityIds.get(intersectionId),
                          intersectionId, "signal changes");
    signalTimers.set(intersectionId, 30.0);
}

void TrafficSim::handleDeparture(Event *eventData)
{
    int intersectionId = eventData->locationId;
    Entity *car = engine->getEntity(eventData->entityId);
    if (car == nullptr)
    {
        return;
    }

    car->state = FINISHED;
    car->timeOfDeparture = eventData->time;
    totalPassed++;

    if (intersectionId >= 0 && intersectionId < numberOfIntersections)
    {
        int passedCount = carsPassedPerIntersection.get(intersectionId) + 1;
        carsPassedPerIntersection.set(intersectionId, passedCount);

        if (carsPassedPerIntersection.get(busiestIntersection) < passedCount)
        {
            busiestIntersection = intersectionId;
        }
    }
}

void TrafficSim::updateBusiestByQueue(int intersectionId)
{
    if (intersectionId < 0 || intersectionId >= numberOfIntersections)
    {
        return;
    }

    int thisCount = waitingCarIds.get(intersectionId)->size();
    int bestCount = waitingCarIds.get(busiestIntersection)->size();
    if (thisCount > bestCount)
    {
        busiestIntersection = intersectionId;
    }
}

double TrafficSim::getRandomCarGap() const
{
    int randomValue = 3 + (std::rand() % 10);
    return (double)randomValue;
}
