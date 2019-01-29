
#include "Simulation.h"

//------------------
//--- Simulation ---
//------------------

std::vector<Cell *> Simulation::Cells()
{
  std::vector<Cell *> res(CellsAndSeeds().size());

  for (size_t i = 0; i < CellsAndSeeds().size(); i++)
    res[i] = CellsAndSeeds()[i].first;

  return res;
}

void Simulation::SaveStates()
{
  if (_sim->CellCount() != _streams.size())
    throw std::runtime_error("Internal error at Simulation::SaveStates() - wrong count of time streams");

  const std::vector<std::pair<Cell *, uint32_t *> > &cells = _sim->Cells();
  for (size_t i = 0; i < _streams.size(); i++)
    _streams[i]->Append(cells[i].first, _sim->Time(), *cells[i].second);
}

Simulation::~Simulation()
{
  if (_sim != NULL)
    delete _sim;
  _sim = NULL;

  for (size_t i = 0; i < _streams.size(); i++)
    if (_streams[i] != NULL) delete _streams[i];
  _streams.clear();
}
