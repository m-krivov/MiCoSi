
#include "TimePredictor.h"

#include "Mitosis.Core/Defs.h"
#include "Mitosis.Core/Timer.h"

//---------------------
//--- TimePredictor ---
//---------------------

TimePredictor::TimePredictor(double startModelTime, double totalModelTime)
  : _startModelTime(startModelTime), _totalModelTime(totalModelTime), _startRealTime(Timer::GetRef()->GetSeconds())
{ /*nothing*/ }

double TimePredictor::ElapsedTime()
{
  return Timer::GetRef()->GetSeconds() - _startRealTime;
}

bool TimePredictor::CanPredictTotalTime(double modelTime, double &predictedRealTime)
{
  double elapsed = ElapsedTime();
  if (modelTime <= _startModelTime)
    return false;

  predictedRealTime = elapsed * (_totalModelTime - _startModelTime) / (modelTime - _startModelTime);
  return true;
}
