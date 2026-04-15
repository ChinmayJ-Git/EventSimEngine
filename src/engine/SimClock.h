#ifndef SIMCLOCK_H
#define SIMCLOCK_H

class SimClock
{
private:
  double currentTime;
  double endTime;
  double startTime;

public:
  SimClock(double endTimeValue, double startTimeValue = 0.0)
  {
    startTime = startTimeValue;
    currentTime = startTimeValue;
    endTime = endTimeValue;
  }

  void advance(double t)
  {
    if (t > currentTime)
    {
      currentTime = t;
    }
  }

  void reset() { currentTime = startTime; }
  bool hasSimulationEnded() const { return currentTime >= endTime; }
  double getCurrentTime() const { return currentTime; }
  double getEndTime() const { return endTime; }
};

#endif