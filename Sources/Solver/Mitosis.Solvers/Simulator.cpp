
#include "Simulator.h"

#include "Mitosis.Core/SimParams.h"
#include "Mitosis.Objects/Cell.h"

//-----------------
//--- Simulator ---
//-----------------

void Simulator::Init(std::vector<std::pair<Cell *, uint32_t *> > &cells, double startTime)
{
  _cells = cells;
  Bind(_cells);
  cells.clear();

  _time = startTime;
  _cellsAreValid = true;
  _statsAreValid = false;
}

const std::vector<std::pair<Cell *, uint32_t *> > &Simulator::Cells()
{
  if (!_cellsAreValid)
  {
    SynchronizeCells();
    _cellsAreValid = true;
  }

  return _cells;
}

const std::vector<CellStats> &Simulator::Stats()
{
  if (!_statsAreValid)
  {
    _stats.clear();
    FormatStats(_stats);
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
    throw std::runtime_error("Internal error at Simulator::DoIteration() - simulation is finished, new iteration cannot be done");

  IterationStarted();
  DoPoleUpdatingStep(Time());
  DoMacroStep(Time());
  DoMicroStep(Time());
  DoSpringBreakingStep(Time());
  IterationFinished();

  _time += GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Dt, true);
  _cellsAreValid = false;
  _statsAreValid = false;
}

Simulator::~Simulator()
{
  for (size_t i = 0; i < _cells.size(); i++)
  {
    if (_cells[i].first != NULL)
      delete _cells[i].first;

    if (_cells[i].second != NULL)
      delete _cells[i].second;
  }
  _cells.clear();
}
