#include "Pole.h"

//------------
//--- Pole ---
//------------

Pole::Pole(uint32_t ID, const CellData *data)
	: _ID(ID),
	  _arr_positions((real *)data->GetArray(CellArray::POLE_POSITION)),
	  _arr_types((uint32_t *)data->GetArray(CellArray::POLE_TYPE))
{
	if (ID >= 2)
  { throw std::runtime_error("Internal error - pole's ID can be equal only to 0 or 1"); }
}
