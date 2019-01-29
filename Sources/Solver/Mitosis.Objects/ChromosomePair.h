
#pragma once

#include "Interfaces.h"
#include "CellData.h"

// Describes the unsplitted pair of the chromosomes.
class ChromosomePair
{
  private:
    const ICellObjectProvider *_objects;

    uint32_t _ID;
    uint32_t *_arr_left_chr, *_arr_right_chr;

  public:
    ChromosomePair(uint32_t ID, const ICellObjectProvider *objects, const CellData *data);

    inline uint32_t ID() const
    { return _ID; }

    inline Chromosome *LeftChromosome()
    { return _objects->GetChromosome(_arr_left_chr[_ID]); }

    inline Chromosome *RightChromosome()
    { return _objects->GetChromosome(_arr_right_chr[_ID]); }

    inline Spring *GetSpring()
    { return _objects->GetSpring(_ID); }
};
