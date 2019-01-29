
#pragma once

#include "Mitosis.Core/Defs.h"

#include "Pole.h"

// Definition of the used classes.
class Cell;
class CellData;
class MT;
class Chromosome;
class ChromosomePair;
class Spring;

// Interface that allows to acquire cell object by its ID.
class ICellObjectProvider
{
  public:
    virtual Pole *GetPole(uint32_t ID) const = 0;

    virtual MT *GetMT(uint32_t ID) const = 0;

    virtual Chromosome *GetChromosome(uint32_t ID) const = 0;

    virtual ChromosomePair *GetChromosomePair(uint32_t ID) const = 0;

    virtual Spring *GetSpring(uint32_t ID) const = 0;

    virtual ~ICellObjectProvider() { }
};

// Interface that describes constructed cell. Probably, not initialized.
class ICell
{
  public:
    virtual Pole *GetPole(PoleType::Type pole) = 0;

    virtual bool AreSpringsBroken() const = 0;

    virtual void SetSpringFlag(bool broken) = 0;

    virtual const std::vector<MT *> &MTs() const = 0;

    virtual const std::vector<Chromosome *> &Chromosomes() const = 0;

    virtual const std::vector<ChromosomePair *> &ChromosomePairs() const = 0;

    virtual ~ICell() { }
};



// Interface that performs cell initialization.
// All returned objects will be destroyed by caller.
class ICellInitializer : public IClonnable
{
  public:
    // Returns essential information about cell.
    virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole) = 0;

    // Creates and returns configured cell object.
    // Don't forget that poles must be also initialized via PoleUpdater!
    virtual void InitializeCell(ICell *cell, uint32_t &seed) = 0;

    virtual ~ICellInitializer() { }
};

// Updates pole's states.
class IPoleUpdater : public IClonnable
{
  public:
    // Sets initial position of the poles.
    virtual void SetInitial(Pole *left, Pole *right, uint32_t &seed) = 0;

    // Changes position of the poles.
    // Can throw std::exception.
    virtual void MovePoles(Pole *left, Pole *right, real time, uint32_t &seed) = 0;

    virtual ~IPoleUpdater() { }
};
