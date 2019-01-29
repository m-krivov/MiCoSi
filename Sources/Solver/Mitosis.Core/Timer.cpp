
#include "Timer.h"

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

//-------------
//--- Timer ---
//-------------

Timer *Timer::_ref = new Timer();

double Timer::GetSeconds()
{
#ifdef WIN32
  static double coeff = -1.0;
  static LARGE_INTEGER count;
  if (coeff <= 0.0)
  {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    coeff = 1.0 / freq.QuadPart;
  }
  QueryPerformanceCounter(&count);
  return coeff * count.QuadPart;
#else
  timeval t;
  gettimeofday(&t, 0);
  return t.tv_sec + (double)t.tv_usec / 1e6;
#endif
}
