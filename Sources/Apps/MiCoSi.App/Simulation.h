#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Streams/All.h"
#include "MiCoSi.Solvers/All.h"

// Definition of the used class
class WorkingDirUtility;

// Extension of the Solver class that adds time stamp and save-to-file functionality
class Simulation : public ICellStatsProvider
{
  public:
    Simulation() = delete;
    Simulation(const Simulation &) = delete;
    Simulation(std::unique_ptr<Simulator> &sim,
               std::vector<std::unique_ptr<TimeStream> > &streams)
      : _sim(std::move(sim)), _streams(std::move(streams))
    { /*nothing*/ }
    Simulation &operator =(const Simulation &) = delete;
    ~Simulation() = default;

    const Simulator::CellEnsemble &CellsAndRng() { return _sim->Cells(); }

    const std::vector<Cell *> Cells();

    double Time() { return _sim->Time(); }

    virtual const std::vector<CellStats> &Stats() override { return _sim->Stats(); }

    bool IsFinished() { return _sim->IsFinished(); }

    void DoIteration() { _sim->DoIteration(); }

    void SaveStates();

  private:
    std::unique_ptr<Simulator> _sim;
    std::vector<std::unique_ptr<TimeStream> > _streams;

  friend class WorkingDirUtility;
};
