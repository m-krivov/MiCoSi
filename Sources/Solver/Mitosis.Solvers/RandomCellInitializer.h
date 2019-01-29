
#pragma once

#include "Mitosis.Objects/Interfaces.h"

// Reads parameters from SimParams and initializes cell using random values.
class RandomCellInitialzier : public ICellInitializer
{
  public:
    // ICellInitializer member.
    virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole);

    // ICellInitializer member.
    virtual void InitializeCell(ICell *cell, uint32_t &seed);

    virtual IClonnable *Clone()
    { return new RandomCellInitialzier(); }
};
