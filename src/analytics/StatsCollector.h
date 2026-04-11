#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include "../core/DynamicArray.h"

#include <iostream>

class StatsCollector
{
public:
  int totalPatients;
  int totalServed;
  int totalEscalations;
  double totalWaitTime;
  double longestWait;
  int totalCars;
  int totalPassed;
  double totalCarWaitTime;
  double longestCarWait;

private:
  DynamicArray<int> historyTotalPatients;
  DynamicArray<int> historyTotalServed;
  DynamicArray<int> historyTotalEscalations;
  DynamicArray<double> historyAveragePatientWait;
  DynamicArray<double> historyLongestWait;
  DynamicArray<int> historyTotalCars;
  DynamicArray<int> historyTotalPassed;
  DynamicArray<double> historyAverageCarWait;
  DynamicArray<double> historyLongestCarWait;

public:
  StatsCollector()
  {
    clearCurrentRun();
  }

  void recordPatientWait(double waitTime)
  {
    totalWaitTime = totalWaitTime + waitTime;
    if (waitTime > longestWait)
    {
      longestWait = waitTime;
    }
  }

  void recordEscalation()
  {
    totalEscalations++;
  }

  void recordPatientServed()
  {
    totalServed++;
    if (totalServed > totalPatients)
    {
      totalPatients = totalServed;
    }
  }

  void recordCarWait(double waitTime)
  {
    totalCarWaitTime = totalCarWaitTime + waitTime;
    if (waitTime > longestCarWait)
    {
      longestCarWait = waitTime;
    }
  }

  void recordCarPassed()
  {
    totalPassed++;
    if (totalPassed > totalCars)
    {
      totalCars = totalPassed;
    }
  }

  void reset()
  {
    bool hasData = false;
    if (totalPatients > 0 || totalServed > 0 || totalEscalations > 0)
    {
      hasData = true;
    }
    if (totalCars > 0 || totalPassed > 0)
    {
      hasData = true;
    }
    if (totalWaitTime > 0.0 || totalCarWaitTime > 0.0)
    {
      hasData = true;
    }

    if (hasData)
    {
      historyTotalPatients.pushBack(totalPatients);
      historyTotalServed.pushBack(totalServed);
      historyTotalEscalations.pushBack(totalEscalations);
      historyAveragePatientWait.pushBack(getAveragePatientWait());
      historyLongestWait.pushBack(longestWait);
      historyTotalCars.pushBack(totalCars);
      historyTotalPassed.pushBack(totalPassed);
      historyAverageCarWait.pushBack(getAverageCarWait());
      historyLongestCarWait.pushBack(longestCarWait);
    }

    clearCurrentRun();
  }

  double getAveragePatientWait() const
  {
    if (totalServed == 0)
    {
      return 0.0;
    }
    return totalWaitTime / (double)totalServed;
  }

  double getAverageCarWait() const
  {
    if (totalPassed == 0)
    {
      return 0.0;
    }
    return totalCarWaitTime / (double)totalPassed;
  }

  void printSummary() const
  {
    std::cout << "Stats summary" << std::endl;
    std::cout << "Patients total: " << totalPatients << std::endl;
    std::cout << "Patients served: " << totalServed << std::endl;
    std::cout << "Escalations total: " << totalEscalations << std::endl;
    std::cout << "Average patient wait: " << getAveragePatientWait() << std::endl;
    std::cout << "Longest patient wait: " << longestWait << std::endl;
    std::cout << "Cars total: " << totalCars << std::endl;
    std::cout << "Cars passed: " << totalPassed << std::endl;
    std::cout << "Average car wait: " << getAverageCarWait() << std::endl;
    std::cout << "Longest car wait: " << longestCarWait << std::endl;
    std::cout << "Stored runs: " << historyTotalPatients.size() << std::endl;
  }

private:
  void clearCurrentRun()
  {
    totalPatients = 0;
    totalServed = 0;
    totalEscalations = 0;
    totalWaitTime = 0.0;
    longestWait = 0.0;
    totalCars = 0;
    totalPassed = 0;
    totalCarWaitTime = 0.0;
    longestCarWait = 0.0;
  }
};

#endif
