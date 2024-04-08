#include "Spring.h"

#include "Chromosome.h"

//--------------
//--- Spring ---
//--------------

Spring::Spring(uint32_t chrPairID, const ICellObjectProvider *objects, const CellData *data)
  : _ID(chrPairID), _objects(objects),
    _arr_left_chr((uint32_t *)data->GetArray(CellArray::CHR_PAIR_LEFF_CHROMOSOME)),
    _arr_right_chr((uint32_t *)data->GetArray(CellArray::CHR_PAIR_RIGHT_CHROMOSOME))
{ /*nothing*/ }

vec3r Spring::LeftJoint() const
{
  return (vec3r)_objects->GetChromosome(_arr_left_chr[_ID])->Position();
}

vec3r Spring::RightJoint() const
{
  return (vec3r)_objects->GetChromosome(_arr_right_chr[_ID])->Position();
}
