#pragma once
#include "MiCoSi.Core/Defs.h"

class TimePredictor
{
  public:
    TimePredictor() = delete;
    TimePredictor(double startModelTime, double totalModelTime);
    TimePredictor(const TimePredictor &) = delete;
    TimePredictor &operator =(const TimePredictor &) = delete;

    double ElapsedTime();

    bool CanPredictTotalTime(double modelTime, double &predictedRealTime);

  private:
    double _startModelTime, _totalModelTime;
    double _startRealTime;
};
