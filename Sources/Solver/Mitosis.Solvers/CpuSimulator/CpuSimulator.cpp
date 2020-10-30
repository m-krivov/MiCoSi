
#include "CpuSimulator.h"

#include <omp.h>

//--------------------
//--- CpuSimulator ---
//--------------------

CpuSimulator::CpuSimulator(IPoleUpdater &updater, size_t num_threads)
  : _updater(updater.CloneTemplated<IPoleUpdater>()), _omp_num_threads((int)num_threads)
{
  if (_omp_num_threads <= 0)
  { _omp_num_threads = std::max(1, omp_get_num_procs()); }
}

void CpuSimulator::Bind(const std::vector<std::pair<Cell *, uint32_t *> > &cells)
{
  _cells = cells;
}

void CpuSimulator::DoMacroStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoMacroStep(_cells[i].first, *_cells[i].second);
  }
}

void CpuSimulator::DoMicroStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoMicroStep(_cells[i].first, *_cells[i].second);
  }
}

void CpuSimulator::DoPoleUpdatingStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoPoleUpdatingStep(_cells[i].first, *_cells[i].second, _updater.get(), time);
  }
}

void CpuSimulator::DoSpringBreakingStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoSpringBreakingStep(_cells[i].first, *_cells[i].second);
  }
}

void CpuSimulator::SynchronizeCells()
{
  // Nothing
}

void CpuSimulator::FormatStats(std::vector<CellStats> &stats)
{
  stats.clear();
  for (size_t i = 0; i < _cells.size(); i++)
    stats.push_back(CellStats::Create(_cells[i].first));
}
