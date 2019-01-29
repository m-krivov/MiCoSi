
#pragma once

#include "Mitosis.Core/Defs.h"

// Definitions of the used classes
class Cell;
class CellStats;
class ICellStatsProvider;
class Version;
class HardwareInfo;

// Formats messages for output stream.
class IOutputFormatter
{
  public:
    virtual void PrintSystemInfo(const HardwareInfo &info,
                   const Version &programVersion,
                   int fileFormatVersion,
                   bool cudaSupport) = 0;

    virtual void PrintRepairStarted(const char *filename) = 0;

    virtual void PrintRepairCompleted(size_t layers, double lastTime) = 0;

    virtual void PrintOnStart(const std::vector<Cell *> &cells) = 0;

    virtual void PrintIterationInfo(ICellStatsProvider *stats, double curSimTime, double totalSimTime) = 0;

    virtual void PrintOnFinish(const std::vector<Cell *> &cells) = 0;

    virtual ~IOutputFormatter() { /*nothing*/ }
};
