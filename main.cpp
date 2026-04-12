#include "src/analytics/CSVExporter.h"
#include "src/analytics/StatsCollector.h"
#include "src/engine/SimEngine.h"
#include "src/gui/Menu.h"
#include "src/gui/Renderer.h"
#include "src/gui/Window.h"
#include "src/scenarios/HospitalSim.h"
#include "src/scenarios/TrafficSim.h"

#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
  Window window(1280, 720, "EventSimEngine");

  sf::RenderWindow &renderWindow = window.getWindow();

  Menu menu;
  int choice = menu.show(renderWindow);

  int numDoctors = 3;
  int numBeds = 10;
  int numIntersections = 6;
  double simDuration = 500.0;
  double arrivalGap = 10.0;
  double carArrivalGap = 5.0;

  if (choice == 1)
  {
    std::cout << "Enter number of doctors (recommended: 2-5): ";
    std::cin >> numDoctors;

    std::cout << "Enter number of beds (recommended: 5-20): ";
    std::cin >> numBeds;

    std::cout << "Enter simulation duration (recommended: 200-1000): ";
    std::cin >> simDuration;

    std::cout << "Enter average seconds between patient arrivals (recommended: 5-15): ";
    std::cin >> arrivalGap;
  }

  if (choice == 2)
  {
    std::cout << "Enter number of intersections (recommended: 4-8): ";
    std::cin >> numIntersections;

    std::cout << "Enter simulation duration (recommended: 200-1000): ";
    std::cin >> simDuration;

    std::cout << "Enter average seconds between car arrivals (recommended: 3-10): ";
    std::cin >> carArrivalGap;
  }

  SimEngine engine(0.0, simDuration, false);

  if (choice == 1)
  {
    HospitalSim hospitalSim(&engine, numDoctors, numBeds, simDuration, arrivalGap);
    hospitalSim.initialise();
    hospitalSim.run();
    hospitalSim.printResults();
  }

  if (choice == 2)
  {
    TrafficSim trafficSim(&engine, numIntersections, simDuration, carArrivalGap);
    trafficSim.initialise();
    trafficSim.run();
    trafficSim.printResults();
  }

  SimulationStats stats = engine.getStats();
  Renderer renderer(renderWindow);

  window.clear();
  if (choice == 1)
  {
    renderer.drawHospital(&engine, numDoctors);
  }
  if (choice == 2)
  {
    renderer.drawTraffic(&engine, numIntersections);
  }
  renderer.drawStats(stats);
  window.display();

  // stay open until user closes
  while (window.isOpen())
  {
    window.pollEvents();
  }

  StatsCollector collector;
  if (choice == 1)
  {
    collector.totalPatients = stats.totalEntitiesCreated;
    collector.totalServed = stats.totalEntitiesFinished;
    collector.totalEscalations = 0;
    collector.totalWaitTime = stats.totalWaitTime;
    collector.longestWait = stats.longestWaitTime;
    collector.totalCars = 0;
    collector.totalPassed = 0;
    collector.totalCarWaitTime = 0.0;
    collector.longestCarWait = 0.0;
  }
  if (choice == 2)
  {
    collector.totalPatients = 0;
    collector.totalServed = 0;
    collector.totalEscalations = 0;
    collector.totalWaitTime = 0.0;
    collector.longestWait = 0.0;
    collector.totalCars = stats.totalEntitiesCreated;
    collector.totalPassed = stats.totalEntitiesFinished;
    collector.totalCarWaitTime = stats.totalWaitTime;
    collector.longestCarWait = stats.longestWaitTime;
  }

  CSVExporter exporter;
  exporter.exportResults("results.csv",
                         collector.totalPatients,
                         collector.totalServed,
                         collector.totalEscalations,
                         collector.getAveragePatientWait(),
                         collector.longestWait,
                         collector.totalCars,
                         collector.totalPassed,
                         collector.getAverageCarWait(),
                         collector.longestCarWait);

  return 0;
}