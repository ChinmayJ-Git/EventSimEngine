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
  std::cout << "==============================" << std::endl;
  std::cout << "      EventSimEngine" << std::endl;
  std::cout << "==============================" << std::endl;
  std::cout << "Choose scenario:" << std::endl;
  std::cout << "  1 = Hospital Simulation" << std::endl;
  std::cout << "  2 = Traffic Simulation" << std::endl;
  std::cout << "Enter choice: ";

  int choice = 0;
  std::cin >> choice;

  int numDoctors = 3;
  int numBeds = 10;
  int numIntersections = 6;
  double simDuration = 500.0;
  double arrivalGap = 10.0;
  double carArrivalGap = 5.0;

  if (choice == 1)
  {
    std::cout << std::endl;
    std::cout << "=== HOSPITAL CONFIGURATION ===" << std::endl;
    std::cout << "Enter number of doctors (recommended: 2-5): ";
    std::cin >> numDoctors;
    std::cout << "Enter number of beds (recommended: 5-20): ";
    std::cin >> numBeds;
    std::cout << "Enter simulation duration (recommended: 200-1000): ";
    std::cin >> simDuration;
    std::cout << "Enter average seconds between arrivals (recommended: 5-15): ";
    std::cin >> arrivalGap;
    std::cout << "==============================" << std::endl;
    std::cout << "Starting hospital simulation..." << std::endl;
  }

  if (choice == 2)
  {
    std::cout << std::endl;
    std::cout << "=== TRAFFIC CONFIGURATION ===" << std::endl;
    std::cout << "Enter number of intersections (recommended: 4-8): ";
    std::cin >> numIntersections;
    std::cout << "Enter simulation duration (recommended: 200-1000): ";
    std::cin >> simDuration;
    std::cout << "Enter average seconds between car arrivals (recommended: 3-10): ";
    std::cin >> carArrivalGap;
    std::cout << "=============================" << std::endl;
    std::cout << "Starting traffic simulation..." << std::endl;
  }

  // open window after console input
  Window window(1280, 720, "EventSimEngine");
  sf::RenderWindow &renderWindow = window.getWindow();
  Renderer renderer(renderWindow);

  // create engine
  SimEngine engine(0.0, simDuration, false);

  HospitalSim *hospitalSim = nullptr;
  TrafficSim *trafficSim = nullptr;

  if (choice == 1)
  {
    hospitalSim = new HospitalSim(&engine, numDoctors, numBeds, simDuration, arrivalGap);
    hospitalSim->initialise();
  }

  if (choice == 2)
  {
    trafficSim = new TrafficSim(&engine, numIntersections, simDuration, carArrivalGap);
    trafficSim->initialise();
  }

  // step by step simulation loop
  while (window.isOpen())
  {
    // handle window close button
    window.pollEvents();

    // process one event
    bool moreEvents = engine.processOneEvent();

    // redraw window
    window.clear();
    if (choice == 1)
      renderer.drawHospital(&engine, numDoctors);
    if (choice == 2)
      renderer.drawTraffic(&engine, numIntersections);

    SimulationStats liveStats = engine.getStats();
    renderer.drawStats(liveStats);
    window.display();

    // pause between events — change this number to speed up or slow down
    sf::sleep(sf::milliseconds(5));

    // stop when simulation finishes
    if (!moreEvents)
      break;
  }

  // print final results to console
  if (choice == 1 && hospitalSim != nullptr)
    hospitalSim->printResults();
  if (choice == 2 && trafficSim != nullptr)
    trafficSim->printResults();

  // keep window open until user closes it
  while (window.isOpen())
    window.pollEvents();

  // export to CSV
  SimulationStats stats = engine.getStats();
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

  delete hospitalSim;
  delete trafficSim;

  return 0;
}