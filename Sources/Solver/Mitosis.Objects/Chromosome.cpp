
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

std::vector<MT *> Chromosome::BoundMTs()
{
	std::vector<MT *> res;
	for (size_t i = 0; i < _mtsPerPole * 2; i++)
		if (_arr_bound_mts[i] == ID())
			res.push_back(_objects->GetMT((uint32_t)i));
	return res;
}
