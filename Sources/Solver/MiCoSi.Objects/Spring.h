#pragma once
#include "MiCoSi.Core/Defs.h"

#include "Interfaces.h"
#include "CellData.h"

//Describes connection between two chromosomes.
class Spring
{
  public:
    Spring() = delete;
    Spring(uint32_t chrPairID, const ICellObjectProvider *objects, const CellData *data);
    Spring(const Spring &) = delete;
    Spring &operator =(const Spring &) = delete;

    vec3r LeftJoint() const;

    vec3r RightJoint() const;

    real Length() const
    { return (RightJoint() - LeftJoint()).GetLength(); }

    uint32_t GetID() const
    { return _ID; }

  private:
    const ICellObjectProvider *_objects;

    uint32_t _ID;
    uint32_t *_arr_left_chr, *_arr_right_chr;
};
