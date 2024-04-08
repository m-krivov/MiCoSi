#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/Random.h"
#include "MiCoSi.Objects/Cell.h"

// To perform simulation, we need a Random Number Generator (RNG)
// To perform deterministic simulation, we need to bundle each cell with its personal RNG
// So, this class is just a helper that simplifies code
class CellWithRng
{
  public:
    CellWithRng(ICellInitializer *cellInitializer,
                IPoleUpdater *poleUpdater,
                const Random::State &rng);
    CellWithRng(const Cell &cell,
                const Random::State &rng);

    CellWithRng() = delete;
    CellWithRng(const CellWithRng &) = delete;
    CellWithRng &operator =(const CellWithRng &) = delete;

    // Returns RNG that is associated with cell
    Random::State &Rng() { return _rng; }

    // Returns RNG that is associated with cell
    const Random::State &Rng() const { return _rng; }

    // The cell itself
    Cell &CellObject() { return *_cell.get(); }

    // The cell itself
    const Cell &CellObject() const { return *_cell.get(); }

  private:
    Random::State _rng;
    std::unique_ptr<Cell> _cell;
};
