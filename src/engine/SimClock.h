// This File manages the virtual time in the simulation.

#ifndef SIMCLOCK_H
#define SIMCLOCK_H

class SimClock
{
private:
  double currentTime;
  double startTime;
  double endTime;
  bool isStoppedEarly;

public:
  // This function initialises the clock with a start and end time
  SimClock(double simulationStartTime, double simulationEndTime)
  {
    currentTime = simulationStartTime;
    startTime = simulationStartTime;
    endTime = simulationEndTime;
    isStoppedEarly = false;
  }

  // This function jumps the clock to the time of the next event
  void advance(double newTime)
  {
    if (newTime > currentTime)
    {
      currentTime = newTime;
    }
  }

  // This function returns the current simulation time
  double getCurrentTime() const
  {
    return currentTime;
  }

  double getStartTime() const
  {
    return startTime;
  }

  double getEndTime() const
  {
    return endTime;
  }

  // This function checks if we have passed the end time
  bool hasSimulationEnded() const
  {
    if (currentTime >= endTime)
    {
      return true;
    }

    if (isStoppedEarly)
    {
      return true;
    }

    return false;
  }

  // This function forces the simulation to stop
  void stopEarly()
  {
    isStoppedEarly = true;
  }

  // This function resets everything so we can run again
  void reset()
  {
    currentTime = startTime;
    isStoppedEarly = false;
  }

  // This function calculates time elapsed since simulation began

  double getElapsedTime() const
  {
    return currentTime - startTime;
  }
};

#endif