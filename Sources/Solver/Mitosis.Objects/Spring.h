
#pragma once

#include "Interfaces.h"
#include "CellData.h"

//Describes connection between two chromosomes.
class Spring
{
  private:
    const ICellObjectProvider *_objects;

    uint32_t _ID;
    uint32_t *_arr_left_chr, *_arr_right_chr;

  public:
    Spring(uint32_t chrPairID, const ICellObjectProvider *objects, const CellData *data);

    vec3r LeftJoint() const;

    vec3r RightJoint() const;

    inline real Length() const
    { return (RightJoint() - LeftJoint()).GetLength(); }

    inline uint32_t GetID() const
    { return _ID; }
};
