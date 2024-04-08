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

void CpuSimulator::Import(CpuSimulator::CellEnsemble &cells)
{
  _cells = std::move(cells);
}

void CpuSimulator::DoMacroStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoMacroStep(_cells[i]->CellObject(), _cells[i]->Rng());
  }
}

void CpuSimulator::DoMicroStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoMicroStep(_cells[i]->CellObject(), _cells[i]->Rng());
  }
}

void CpuSimulator::DoPoleUpdatingStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoPoleUpdatingStep(_cells[i]->CellObject(), _cells[i]->Rng(), _updater.get(), time);
  }
}

void CpuSimulator::DoSpringBreakingStep(double time)
{
#pragma omp parallel for num_threads(NumThreads())
  for (int i = 0; i < (int)_cells.size(); i++)
  {
    DoSpringBreakingStep(_cells[i]->CellObject(), _cells[i]->Rng());
  }
}

const CpuSimulator::CellEnsemble &CpuSimulator::SynchronizeCells()
{
  return _cells;
}

void CpuSimulator::FormatStats(std::vector<CellStats> &stats)
{
  stats.clear();
  for (size_t i = 0; i < _cells.size(); i++)
  { stats.push_back(CellStats::Create(&_cells[i]->CellObject())); }
}
