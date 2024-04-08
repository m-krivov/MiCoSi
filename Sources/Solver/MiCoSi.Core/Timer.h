#pragma once
#include "Defs.h"

// Static class that provides current time
class Timer
{
  public:
    Timer(const Timer &) = delete;
    Timer &operator =(const Timer &) = delete;

    double GetSeconds();

    double GetMilliSeconds()
    { return GetSeconds() / 1000.0; }

    static Timer *GetRef()
    { return _ref; }

  private:
    Timer() = default;
    static Timer *_ref;
};
