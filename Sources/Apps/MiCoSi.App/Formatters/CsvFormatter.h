#pragma once
#include "MiCoSi.Core/Defs.h"

#include "IErrorFormatter.h"
#include "IOutputFormatter.h"
#include "TimePredictor.h"


class CsvFormatter : public IErrorFormatter, public IOutputFormatter
{
  public:
    CsvFormatter(double delay)
      : _delay(delay), _prevTime(-delay * 2)
    { /*nothing*/ }

    virtual void PrintSystemInfo(const HardwareInfo &info,
                                 const Version &programVersion,
                                 int fileFormatVersion,
                                 bool cudaSupport);

    virtual void PrintError(IErrorFormatter::ErrorType type, const char *errDesc);

    virtual void PrintRepairStarted(const char *filename) { /*nothing*/ }

    virtual void PrintRepairCompleted(size_t layers, double lastTime) { /*nothing*/ }

    virtual void PrintOnStart(const std::vector<Cell *> &cells);

    virtual void PrintIterationInfo(ICellStatsProvider *stats, double curSimTime, double totalSimTime);

    virtual void PrintOnFinish(const std::vector<Cell *> &cells) { /*nothing*/ }

  private:
    std::unique_ptr<TimePredictor> _predictor;
    double _delay, _prevTime;
};
