#include "../src/engine/SimEngine.h"
#include <iostream>

int main()
{
  std::cout << "Running engine test..." << std::endl;

  // create engine — runs from t=0 to t=300
  SimEngine testEngine(0.0, 300.0, true);

  // create patients
  Entity *patient1 = new Entity(1, PATIENT, "Patient_1", 0.0, 3);
  Entity *patient2 = new Entity(2, PATIENT, "Patient_2", 0.0, 1);
  Entity *patient3 = new Entity(3, PATIENT, "Patient_3", 0.0, 5);

  testEngine.addEntity(patient1);
  testEngine.addEntity(patient2);
  testEngine.addEntity(patient3);

  // schedule arrivals
  testEngine.scheduleEvent(5.0, ARRIVAL, 1, 1, "Patient_1 arrives");
  testEngine.scheduleEvent(12.0, ARRIVAL, 2, 1, "Patient_2 arrives");
  testEngine.scheduleEvent(20.0, ARRIVAL, 3, 1, "Patient_3 arrives");

  // schedule escalation events
  // patient 3 is MINOR priority — escalates if not seen by t=50
  testEngine.scheduleEvent(50.0, ESCALATION, 3, 1, "Patient_3 priority escalates");

  // schedule service events
  testEngine.scheduleEvent(10.0, SERVICE_START, 1, 2, "Doctor starts Patient_1");
  testEngine.scheduleEvent(35.0, DEPARTURE, 1, 2, "Patient_1 discharged");
  testEngine.scheduleEvent(36.0, SERVICE_START, 2, 2, "Doctor starts Patient_2");
  testEngine.scheduleEvent(55.0, DEPARTURE, 2, 2, "Patient_2 discharged");
  testEngine.scheduleEvent(56.0, SERVICE_START, 3, 2, "Doctor starts Patient_3");
  testEngine.scheduleEvent(80.0, DEPARTURE, 3, 2, "Patient_3 discharged");

  // schedule signal check — adaptive traffic logic
  testEngine.scheduleEvent(100.0, SIGNAL_CHECK, 0, 5, "Intersection 5 checks queue lengths");
  testEngine.scheduleEvent(110.0, SIGNAL_CHANGE, 0, 5, "Intersection 5 signal changes");

  // run
  testEngine.run();
  testEngine.printStats();

  // verify
  bool allFinished = (testEngine.getStats().totalEntitiesFinished == 3);
  bool allProcessed = (testEngine.getStats().totalEventsProcessed == 12);

  if (allFinished)
    std::cout << "PASS: all 3 patients finished." << std::endl;
  else
    std::cout << "FAIL: expected 3 finished, got "
              << testEngine.getStats().totalEntitiesFinished << std::endl;

  if (allProcessed)
    std::cout << "PASS: all 11 events processed." << std::endl;
  else
    std::cout << "FAIL: expected 11 events, got "
              << testEngine.getStats().totalEventsProcessed << std::endl;

  delete patient1;
  delete patient2;
  delete patient3;

  return 0;
}