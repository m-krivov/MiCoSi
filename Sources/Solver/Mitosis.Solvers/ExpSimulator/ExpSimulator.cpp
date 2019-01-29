
#include "ExpSimulator.h"

//--------------------
//--- ExpSimulator ---
//--------------------

void ExpSimulator::Bind(const std::vector<std::pair<Cell *, uint32_t *> > &cells)
{
  _cells = cells;
}

void ExpSimulator::DoMacroStep(double time)
{
  for (size_t i = 0; i < _cells.size(); i++)
    DoMacroStep(_cells[i].first, *_cells[i].second);
}

void ExpSimulator::DoMicroStep(double time)
{
  for (size_t i = 0; i < _cells.size(); i++)
    DoMicroStep(_cells[i].first, *_cells[i].second);
}

void ExpSimulator::DoPoleUpdatingStep(double time)
{
  for (size_t i = 0; i < _cells.size(); i++)
    DoPoleUpdatingStep(_cells[i].first, *_cells[i].second, _updater.get(), time);
}

void ExpSimulator::DoSpringBreakingStep(double time)
{
  for (size_t i = 0; i < _cells.size(); i++)
    DoSpringBreakingStep(_cells[i].first, *_cells[i].second);
}

void ExpSimulator::SynchronizeCells()
{
  // Nothing
}

void ExpSimulator::FormatStats(std::vector<CellStats> &stats)
{
  stats.clear();
  for (size_t i = 0; i < _cells.size(); i++)
    stats.push_back(CellStats::Create(_cells[i].first));
}
