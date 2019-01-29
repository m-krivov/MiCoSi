
#pragma once

#include "Mitosis.Objects/Interfaces.h"

// Creates required cell structure without initialization of MTs and chromosomes.
class DeSerializingCellInitializer :public ICellInitializer
{
  private:
    size_t _chrPairs;
    size_t _mtsPerPole;

  public:
    DeSerializingCellInitializer(size_t chrPairs, size_t mtsPerPole);

    // ICellInitializer member.
    virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole);

    // ICellInitializer member.
    virtual void InitializeCell(ICell *cell, uint32_t &seed);

    // IClonnable member.
    virtual IClonnable *Clone()
    { return new DeSerializingCellInitializer(_chrPairs, _mtsPerPole); }
};
