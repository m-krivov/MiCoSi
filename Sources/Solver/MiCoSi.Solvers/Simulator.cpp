#include "Simulator.h"

#include "MiCoSi.Core/SimParams.h"
#include "MiCoSi.Objects/Cell.h"

//-----------------
//--- Simulator ---
//-----------------

void Simulator::Init(std::vector<std::unique_ptr<CellWithRng> > &cells, double startTime)
{
  if (cells.size() == 0)
  { throw std::runtime_error("cannot initialize solver without cells"); }
  
  _cellCount = cells.size();
  _time = startTime;
  _statsAreValid = false;

  Import(cells);
}

const std::vector<std::unique_ptr<CellWithRng> > &Simulator::Cells()
{
  decltype(auto) cells = SynchronizeCells();
  if (cells.size() != _cellCount)
  { throw std::runtime_error("internal error, solver has returned wrong number of cells"); }

  return cells;
}

const std::vector<CellStats> &Simulator::Stats()
{
  if (!_statsAreValid)
  {
    _stats.clear();
    FormatStats(_stats);
    if (_stats.size() != _cellCount)
    { throw std::runtime_error("internal error, solver has returned wrong number of cells"); }

    _statsAreValid = true;
  }

  return _stats;
}

bool Simulator::IsFinished()
{
  double cur = Time();
  double end = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::T_End, true);
  return cur >= end || std::abs(cur - end) < 1e-6;
}

void Simulator::DoIteration()
{
  if (IsFinished())
  { throw std::runtime_error("internal error: simulation is finished and new iteration cannot be done"); }

  IterationStarted();
  DoPoleUpdatingStep(Time());
  DoMacroStep(Time());
  DoMicroStep(Time());
  DoSpringBreakingStep(Time());
  IterationFinished();

  _time += GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Dt, true);
  _statsAreValid = false;
}
