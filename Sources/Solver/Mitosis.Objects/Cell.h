
#pragma once
#include "Mitosis.Core/Defs.h"

#include "Mitosis.Core/Random.h"

#include "Interfaces.h"
#include "CellData.h"
#include "Pole.h"

// Object-oriented wrapper that describes the whole cell
// Doesn't cache values, any changes are being applied directly to the base "CellData" object
class Cell : public ICell, public IClonnable, private ICellObjectProvider
{
  private:
    std::unique_ptr<CellData> _data;

    Pole *_poles[2];
    std::vector<MT *> _MTs;
    std::vector<Chromosome *> _chromosomes;
    std::vector<ChromosomePair *> _chromosomePairs;
    std::vector<Spring *> _springs;
    uint32_t *_springs_broken_flag;

  private:
    Cell(CellData *data);

    virtual Pole *GetPole(uint32_t ID) const
    { return _poles[ID]; }

    virtual MT *GetMT(uint32_t ID) const
    { return _MTs[ID]; }

    virtual Chromosome *GetChromosome(uint32_t ID) const
    { return _chromosomes[ID]; }

    virtual ChromosomePair *GetChromosomePair(uint32_t ID) const
    { return _chromosomePairs[ID]; }

    virtual Spring *GetSpring(uint32_t ID) const
    { return AreSpringsBroken() ? NULL : _springs[ID]; }

    void CreateObjects(CellData *data);

  public:
    Cell(ICellInitializer *initializer, IPoleUpdater *updater, Random::State &state);
    Cell(Cell &) = delete;
    Cell &operator =(Cell &) = delete;

    virtual bool AreSpringsBroken() const
    { return *_springs_broken_flag != 0; }

    virtual void SetSpringFlag(bool broken)
    { *_springs_broken_flag = broken ? 1 : 0; }

    virtual Pole *GetPole(PoleType pole) const override
    { return pole == PoleType::Left ? _poles[(uint32_t)PoleType::Left] : _poles[(uint32_t)PoleType::Right]; }

    virtual const std::vector<MT *> &MTs() const override
    { return _MTs; }

    virtual const std::vector<Chromosome *> &Chromosomes() const override
    { return _chromosomes; }

    virtual const std::vector<ChromosomePair *> &ChromosomePairs() const override
    { return _chromosomePairs; }

    const CellData &Data() const
    { return *_data.get(); }

    virtual IClonnable *Clone() const override;

    ~Cell();
};
