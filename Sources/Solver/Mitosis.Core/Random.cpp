
#include "Random.h"
#include "Timer.h"

//--------------
//--- Random ---
//--------------

uint32_t Random::CreateSeed()
{
  double t = Timer::GetRef()->GetMilliSeconds();
  if (t < 1.0)
  { t = 1.0 / (t + 1e-6); }

  if ((uint32_t)t < Func::MaxRandom())
  { t *= Func::MaxRandom(); }
  return (uint32_t)t;
}
