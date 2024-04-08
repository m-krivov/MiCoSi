#include "CellWithRng.h"

CellWithRng::CellWithRng(ICellInitializer *cellInitializer,
                         IPoleUpdater *poleUpdater,
                         const Random::State &rng)
  : _rng(rng), _cell(new Cell(cellInitializer, poleUpdater, _rng))
{
  // nothing
}

CellWithRng::CellWithRng(const Cell &cell, const Random::State &rng)
  : _rng(rng), _cell(cell.CloneTemplated<Cell>())
{
  // nothing
}
