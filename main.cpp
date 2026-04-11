#include "src/analytics/CSVExporter.h"
#include "src/analytics/StatsCollector.h"
#include "src/engine/SimEngine.h"
#include "src/gui/Menu.h"
#include "src/gui/Renderer.h"
#include "src/gui/Window.h"
#include "src/scenarios/HospitalSim.h"
#include "src/scenarios/TrafficSim.h"

#include <SFML/Graphics.hpp>

int main()
{
  Window window(1280, 720, "EventSimEngine");

  sf::RenderWindow &renderWindow = window.getWindow();

  Menu menu;
  int choice = menu.show(renderWindow);

  SimEngine engine(0.0, 500.0, false);

  if (choice == 1)
  {
    HospitalSim hospitalSim(&engine, 3, 10, 500.0);
    hospitalSim.initialise();
    hospitalSim.run();
    hospitalSim.printResults();
  }

  if (choice == 2)
  {
    TrafficSim trafficSim(&engine, 6, 500.0);
    trafficSim.initialise();
    trafficSim.run();
    trafficSim.printResults();
  }

  SimulationStats stats = engine.getStats();
  Renderer renderer(renderWindow);

  window.clear();
  if (choice == 1)
  {
    renderer.drawHospital(&engine, 3);
  }
  if (choice == 2)
  {
    renderer.drawTraffic(&engine, 6);
  }
  renderer.drawStats(stats);
  window.display();

  sf::Clock waitClock;
  while (window.isOpen() && waitClock.getElapsedTime().asSeconds() < 5.0f)
  {
    window.pollEvents();
  }
  window.close();

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