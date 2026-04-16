#include "src/analytics/CSVExporter.h"
#include "src/gui/Menu.h"
#include "src/gui/Renderer.h"
#include "src/gui/Window.h"
#include "src/scenarios/HospitalSim.h"

int main()
{
  Window window("MediSim Engine", 1280, 720);
  Menu menu;
  if (!menu.isReady())
  {
    return 1;
  }

  SimConfig config = menu.show(window.getWindow());
  if (config.duration <= 0 || !window.checkOpen())
  {
    return 0;
  }

  double arrivalPerSecond = (config.arrivalRate > 0) ? 1.0 / (double)config.arrivalRate : 0.3;
  HospitalSim *sim = new HospitalSim(config.duration, arrivalPerSecond,
                                     config.criticalDoctors, config.generalDoctors, config.specialistDoctors);

  Renderer renderer(&window);
  if (!renderer.isReady())
  {
    delete sim;
    return 1;
  }

  sim->attachLiveView(&window, &renderer);
  sim->setFastMode(config.fastMode);
  sim->run();
  sim->printStats();
  bool csvSaved = CSVExporter::exportRun(sim->getCurrentSimTime(), sim->getTotalPatients(),
                                         sim->getAverageWait(), sim->getLongestWait(), sim->getEscalationCount(),
                                         sim->getDoctorUtilisation(0), sim->getDoctorUtilisation(1),
                                         sim->getDoctorUtilisation(2), sim->getTotalWaiting());
  renderer.setCsvSaved(csvSaved);
  renderer.setSimulationComplete(true);

  while (window.checkOpen())
  {
    window.pollEvents();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
      window.close();
      continue;
    }
    window.clear();
    renderer.drawAll(sim);
    window.display();
  }

  delete sim;
  return 0;
}