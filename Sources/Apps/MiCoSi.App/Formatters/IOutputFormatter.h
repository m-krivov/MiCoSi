#pragma once
#include "MiCoSi.Core/Defs.h"

// Definitions of the used classes
class Cell;
class CellStats;
class ICellStatsProvider;
class Version;
class HardwareInfo;

// Formats messages for output stream
class IOutputFormatter
{
  public:
    IOutputFormatter() = default;
    IOutputFormatter(const IOutputFormatter &) = delete;
    IOutputFormatter &operator =(const IOutputFormatter &) = delete;

    virtual void PrintSystemInfo(const HardwareInfo &info,
                                 const Version &programVersion,
                                 int fileFormatVersion,
                                 bool cudaSupport) = 0;

    virtual void PrintRepairStarted(const char *filename) abstract;

    virtual void PrintRepairCompleted(size_t layers, double lastTime) abstract;

    virtual void PrintOnStart(const std::vector<Cell *> &cells) abstract;

    virtual void PrintIterationInfo(ICellStatsProvider *stats, double curSimTime, double totalSimTime) abstract;

    virtual void PrintOnFinish(const std::vector<Cell *> &cells) abstract;

    virtual ~IOutputFormatter() { /*nothing*/ }
};
