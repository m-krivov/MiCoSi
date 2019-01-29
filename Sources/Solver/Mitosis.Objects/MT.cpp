
#include "MT.h"

#include "Pole.h"
#include "Chromosome.h"

//----------
//--- MT ---
//----------

MT::MT(uint32_t ID, const ICellObjectProvider *objects, const CellData *data)
	: _ID(ID), _objects(objects),
	  _arr_poles((uint32_t *)data->GetArray(CellArray::MT_POLE)),
	  _arr_dir_x((real *)data->GetArray(CellArray::MT_DIRECTION_X)),
	  _arr_dir_y((real *)data->GetArray(CellArray::MT_DIRECTION_Y)),
	  _arr_dir_z((real *)data->GetArray(CellArray::MT_DIRECTION_Z)),
	  _arr_force_x((real *)data->GetArray(CellArray::MT_FORCE_OFFSET_X)),
	  _arr_force_y((real *)data->GetArray(CellArray::MT_FORCE_OFFSET_Y)),
	  _arr_force_z((real *)data->GetArray(CellArray::MT_FORCE_OFFSET_Z)),
	  _arr_lengthes((real *)data->GetArray(CellArray::MT_LENGTH)),
	  _arr_states((uint32_t *)data->GetArray(CellArray::MT_STATE)),
	  _arr_bound_chrs((int32_t *)data->GetArray(CellArray::MT_BOUND_CHROMOSOME))
{ /*nothing*/ }

Chromosome *MT::BoundChromosome()
{
	int32_t chrID = _arr_bound_chrs[_ID];
	return chrID < 0 ? NULL : _objects->GetChromosome(chrID);
}

void MT::Bind(Chromosome *cr)
{
	_arr_bound_chrs[_ID] = cr->ID();
}
