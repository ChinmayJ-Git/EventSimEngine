#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <fstream>
#include <iomanip>

class CSVExporter
{
public:
  static bool exportRun(double runTime, int totalPatients, double avgWaitSeconds,
                        double longestWaitSeconds, int totalEscalations,
                        double drAnjaliUse, double drMaheshUse, double drRahulUse,
                        int patientsStillWaiting)
  {
    bool hasData = false;
    {
      std::ifstream in("results.csv", std::ios::binary | std::ios::ate);
      hasData = in.is_open() && in.tellg() > 0;
    }

    std::ofstream file("results.csv", std::ios::out | std::ios::app);
    if (!file.is_open())
    {
      return false;
    }

    if (!hasData)
    {
      file << "RunTimeSeconds,TotalPatients,AvgWaitSeconds,LongestWaitSeconds,TotalEscalations,";
      file << "EmergencyUtilPct,GeneralUtilPct,SpecialistUtilPct,PatientsStillWaiting\n";
    }

    file << std::fixed << std::setprecision(3)
         << runTime << "," << totalPatients << "," << avgWaitSeconds << ","
         << longestWaitSeconds << "," << totalEscalations << "," << drAnjaliUse << ","
         << drMaheshUse << "," << drRahulUse << "," << patientsStillWaiting << "\n";

    file.flush();
    return file.good();
  }
};

#endif
