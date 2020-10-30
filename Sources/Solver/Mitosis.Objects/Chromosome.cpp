
#include "Chromosome.h"
#include "MT.h"

//------------------
//--- Chromosome ---
//------------------

Chromosome::Chromosome(uint32_t ID, const ICellObjectProvider *objects, const CellData *data)
	: _ID(ID), _objects(objects), _mtsPerPole(data->MTsPerPole()),
	  _arr_pos((real *)data->GetArray(CellArray::CHR_POSITION)),
	  _arr_orient((real *)data->GetArray(CellArray::CHR_ORIENTATION)),
	  _arr_bound_mts((int32_t *)data->GetArray(CellArray::MT_BOUND_CHROMOSOME))
{ /*nothing*/ }
