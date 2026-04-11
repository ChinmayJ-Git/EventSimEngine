#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <fstream>
#include <string>

class CSVExporter
{
public:
  void exportResults(const std::string &filename,
                     int totalPatients, int totalServed,
                     int totalEscalations, double avgWait,
                     double longestWait, int totalCars,
                     int totalPassed, double avgCarWait,
                     double longestCarWait)
  {
    std::ofstream file(filename.c_str());
    if (!file.is_open())
    {
      return;
    }

    file << "totalPatients,totalServed,totalEscalations,avgWait,longestWait,totalCars,totalPassed,avgCarWait,longestCarWait";
    file << "\n";

    file << totalPatients << ","
         << totalServed << ","
         << totalEscalations << ","
         << avgWait << ","
         << longestWait << ","
         << totalCars << ","
         << totalPassed << ","
         << avgCarWait << ","
         << longestCarWait;
    file << "\n";

    file.close();
  }
};

#endif
