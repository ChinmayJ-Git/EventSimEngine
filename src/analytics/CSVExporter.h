#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <fstream>

class CSVExporter
{
public:
  static void exportRun(int runNumber, double avgWait, double longestWait,
                        int escalations, int totalPatients,
                        double utilEmergency, double utilGeneral, double utilSpecialist)
  {
    bool isNew = false;
    std::ifstream check("results.csv");
    if (!check.is_open())
    {
      isNew = true;
    }
    check.close();

    std::ofstream file("results.csv", std::ios::app);
    if (!file.is_open())
    {
      return;
    }
    if (isNew)
    {
      file << "Run,AvgWait,LongestWait,Escalations,TotalPatients,";
      file << "EmergencyUtil,GeneralUtil,SpecialistUtil\n";
    }
    file << runNumber << "," << avgWait << "," << longestWait << ","
         << escalations << "," << totalPatients << "," << utilEmergency << ","
         << utilGeneral << "," << utilSpecialist << "\n";
  }
};

#endif
