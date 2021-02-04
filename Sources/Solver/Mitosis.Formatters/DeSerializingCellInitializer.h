
#pragma once

#include "Mitosis.Objects/Interfaces.h"

// Creates the required cell structure without initialization of MTs and chromosomes
class DeSerializingCellInitializer :public ICellInitializer
{
  public:
    DeSerializingCellInitializer() = delete;
    DeSerializingCellInitializer(size_t chrPairs, size_t mtsPerPole);
    DeSerializingCellInitializer &operator =(const DeSerializingCellInitializer &) = delete;

    // ICellInitializer member
    virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole) override;

    // ICellInitializer member
    virtual void InitializeCell(ICell *cell, Random::State &state) override;

    // IClonnable member
    virtual IClonnable *Clone() const override
    { return new DeSerializingCellInitializer(_chrPairs, _mtsPerPole); }

  private:
    size_t _chrPairs;
    size_t _mtsPerPole;
};
