
#pragma once

#include "Mitosis.Streams/All.h"
#include "Mitosis.Solvers/All.h"

// Definition of the used class.
class WorkingDirUtility;

// Extension of the Solver class that adds saving to file functionality.
class Simulation : public ICellStatsProvider
{
  private:
    Simulator *_sim;
    std::vector<TimeStream *> _streams;

    Simulation(Simulator *sim, std::vector<TimeStream *> &streams)
      : _sim(sim), _streams(streams)
    { /*nothing*/ }

    Simulation(const Simulation &) = delete;
    void operator =(const Simulation &) = delete;

  public:
    std::vector<Cell *> Cells();

    inline const std::vector<std::pair<Cell *, uint32_t *> > &CellsAndSeeds() { return _sim->Cells(); }

    inline double Time() { return _sim->Time(); }

    virtual const std::vector<CellStats> &Stats() { return _sim->Stats(); }

    inline bool IsFinished() { return _sim->IsFinished(); }

    void DoIteration() { _sim->DoIteration(); }

    void SaveStates();

    ~Simulation();

  friend class WorkingDirUtility;
};
