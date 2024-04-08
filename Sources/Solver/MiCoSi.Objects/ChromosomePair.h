#pragma once
#include "MiCoSi.Core/Defs.h"

#include "Interfaces.h"
#include "CellData.h"

// Describes the unsplitted pair of the chromosomes
class ChromosomePair
{
  public:
    ChromosomePair(uint32_t ID, const ICellObjectProvider *objects, const CellData *data);

    uint32_t ID() const
    { return _ID; }

    Chromosome *LeftChromosome()
    { return _objects->GetChromosome(_arr_left_chr[_ID]); }

    Chromosome *RightChromosome()
    { return _objects->GetChromosome(_arr_right_chr[_ID]); }

    Spring *GetSpring()
    { return _objects->GetSpring(_ID); }

  private:
    const ICellObjectProvider *_objects;

    uint32_t _ID;
    uint32_t *_arr_left_chr, *_arr_right_chr;
};
