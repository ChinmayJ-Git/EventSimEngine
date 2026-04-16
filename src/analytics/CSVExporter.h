#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <fstream>

class CSVExporter
{
public:
  static void exportRun(double runTime, int totalPatients, double avgWaitSeconds,
                        double longestWaitSeconds, int totalEscalations,
                        double drAnjaliUse, double drMaheshUse, double drRahulUse,
                        int patientsStillWaiting)
  {
    std::ofstream file("results.csv");
    if (!file.is_open())
    {
      return;
    }
    file << "RunTime,TotalPatients,AvgWaitSeconds,LongestWaitSeconds,TotalEscalations,";
    file << "DrAnjaliUse%,DrMaheshUse%,DrRahulUse%,PatientsStillWaiting\n";
    file << runTime << "," << totalPatients << "," << avgWaitSeconds << ","
         << longestWaitSeconds << "," << totalEscalations << "," << drAnjaliUse << ","
         << drMaheshUse << "," << drRahulUse << "," << patientsStillWaiting << "\n";
  }
};

#endif
