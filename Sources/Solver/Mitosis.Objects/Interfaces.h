
#pragma once

#include "Mitosis.Core/Defs.h"

#include "Pole.h"

// Definition of the used classes
class Cell;
class CellData;
class MT;
class Chromosome;
class ChromosomePair;
class Spring;

// Allows us to acquire cell objects (chromosomes, MTs, etc) by their IDs
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

// Describes the constructed cell
// Probably, it is not initialized yet
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

// Performs cell initialization
class ICellInitializer : public IClonnable
{
  public:
    // Returns the essential information about cell.
    virtual void GetCellConfig(size_t &chrPairs, size_t &mtsPerPole) = 0;

    // Creates and returns configured cell object.
    // Don't forget that poles must be also initialized via PoleUpdater!
    virtual void InitializeCell(ICell *cell, uint32_t &seed) = 0;

    virtual ~ICellInitializer() { }
};

// Updates pole's state
class IPoleUpdater : public IClonnable
{
  public:
    // Sets the initial position of the poles
    virtual void SetInitial(Pole *left, Pole *right, uint32_t &seed) = 0;

    // Changes the position of the poles
    virtual void MovePoles(Pole *left, Pole *right, real time, uint32_t &seed) = 0;

    virtual ~IPoleUpdater() { }
};
