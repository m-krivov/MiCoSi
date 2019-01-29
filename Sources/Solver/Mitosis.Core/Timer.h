
#pragma once

// Static class that provides current time.
class Timer
{
  private:
    static Timer *_ref;
    Timer() { /*nothing*/ };

  public:
    Timer(const Timer &) = delete;
    void operator =(const Timer &) = delete;

    double GetSeconds();

    inline double GetMilliSeconds()
    { return GetSeconds() / 1000.0; }

  public:
    static inline Timer *GetRef()
    { return _ref; }
};
