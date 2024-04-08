#include "ChromosomePair.h"

#include "Chromosome.h"

//----------------------
//--- ChromosomePair ---
//----------------------

ChromosomePair::ChromosomePair(uint32_t ID, const ICellObjectProvider *objects, const CellData *data)
  : _ID(ID), _objects(objects),
    _arr_left_chr((uint32_t *)data->GetArray(CellArray::CHR_PAIR_LEFF_CHROMOSOME)),
    _arr_right_chr((uint32_t *)data->GetArray(CellArray::CHR_PAIR_RIGHT_CHROMOSOME))
{ /*nothing*/ }
