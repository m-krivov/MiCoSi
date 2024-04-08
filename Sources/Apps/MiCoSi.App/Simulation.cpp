
#include "Simulation.h"

//------------------
//--- Simulation ---
//------------------

const std::vector<Cell *> Simulation::Cells()
{
  std::vector<Cell *> res;
  decltype(auto) ensemble = CellsAndRng();
  for (auto &cell : ensemble)
  { res.emplace_back(&cell->CellObject()); }
  return res;
}

void Simulation::SaveStates()
{
  if (_sim->CellCount() != _streams.size())
  { throw std::runtime_error("internal error: wrong count of time streams"); }

  decltype(auto) cells = _sim->Cells();
  for (size_t i = 0; i < _streams.size(); i++)
  { _streams[i]->Append(cells[i]->CellObject(), _sim->Time(), cells[i]->Rng()); }
}
