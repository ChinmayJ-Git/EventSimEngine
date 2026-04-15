#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include <iostream>

class StatsCollector
{
private:
  double avgWaitTimes[10];
  double longestWaits[10];
  int escalationCounts[10];
  double doctorUtilisation[3][10];
  int totalPatientCounts[10];
  int runCount;

public:
  StatsCollector()
  {
    runCount = 0;
    for (int i = 0; i < 10; i++)
    {
      avgWaitTimes[i] = 0.0;
      longestWaits[i] = 0.0;
      escalationCounts[i] = 0;
      totalPatientCounts[i] = 0;
      doctorUtilisation[0][i] = 0.0;
      doctorUtilisation[1][i] = 0.0;
      doctorUtilisation[2][i] = 0.0;
    }
  }

  void recordRun(double avgWait, double longestWait, int escalations,
                 int totalPatients, double utilDoc1, double utilDoc2, double utilDoc3)
  {
    if (runCount >= 10)
    {
      return;
    }
    avgWaitTimes[runCount] = avgWait;
    longestWaits[runCount] = longestWait;
    escalationCounts[runCount] = escalations;
    totalPatientCounts[runCount] = totalPatients;
    doctorUtilisation[0][runCount] = utilDoc1;
    doctorUtilisation[1][runCount] = utilDoc2;
    doctorUtilisation[2][runCount] = utilDoc3;
    runCount++;
  }

  void printComparison()
  {
    std::cout << "Run comparison" << std::endl;
    for (int i = 0; i < runCount; i++)
    {
      std::cout << "Run " << (i + 1)
                << " | Avg " << avgWaitTimes[i]
                << " | Longest " << longestWaits[i]
                << " | Esc " << escalationCounts[i]
                << " | Patients " << totalPatientCounts[i]
                << " | U1 " << doctorUtilisation[0][i]
                << " | U2 " << doctorUtilisation[1][i]
                << " | U3 " << doctorUtilisation[2][i]
                << std::endl;
    }
  }

  int getRunCount() { return runCount; }
};

#endif
