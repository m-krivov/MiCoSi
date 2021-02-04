
#pragma once

#include "Mitosis.Objects/Interfaces.h"

// Reads parameters from SimParams and initializes cell using random values
class RandomCellInitialzier : public ICellInitializer
{
  public:
    // ICellInitializer member
    virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole);

    // ICellInitializer member
    virtual void InitializeCell(ICell *cell, Random::State &state);

    virtual IClonnable *Clone() const override
    { return new RandomCellInitialzier(); }
};
