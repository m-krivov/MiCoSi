
#pragma once

class TimePredictor
{
  private:
    double _startModelTime, _totalModelTime;
    double _startRealTime;

  public:
    TimePredictor(double startModelTime, double totalModelTime);

    double ElapsedTime();

    bool CanPredictTotalTime(double modelTime, double &predictedRealTime);
};
