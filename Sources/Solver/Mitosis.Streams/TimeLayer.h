
#pragma once

#include "Mitosis.Core/SimParams.h"
#include "Mitosis.Objects/Cell.h"

// Light-weight wrapper that contains information about iteration.
class TimeLayer
{
  private:
    Cell *_cell;
    SimParams *_simParams;
    double _time;
    uint32_t _randSeed;

  public:
    // Provided "Cell" and "SimParams" objects will not be destroyed by destructor.
    inline TimeLayer(Cell *cell, SimParams *simParams, double time, uint32_t randSeed)
    {
      _cell = cell;
      _simParams = simParams;
      _time = time;
      _randSeed = randSeed;
    }

    inline SimParams *GetSimParams()
    { return _simParams; }

    inline Cell *GetCell()
    { return _cell; }

    inline double GetTime() const
    { return _time; }

    // Returns the seed at the end of iteration.
    inline uint32_t GetRandSeed() const
    { return _randSeed; }
};
