#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Geometry/mat3x3x.h"
#include "Interfaces.h"
#include "CellData.h"

class Chromosome
{
  public:
    Chromosome() = delete;
    Chromosome(uint32_t ID, const ICellObjectProvider *objects, const CellData *data);
    Chromosome(const Chromosome &) = delete;
    Chromosome &operator =(const Chromosome &) = delete;

    uint32_t ID() const
    { return _ID; }

    vec3r_assigner Position()
    { real *p = _arr_pos + _ID * 3; return vec3r_assigner(p, p + 1, p + 2); }

    const vec3r Position() const
    { real *p = _arr_pos + _ID * 3; return vec3r(p[0], p[1], p[2]); }

    mat3x3r_assigner Orientation()
    { real *p = _arr_orient + _ID * 9; return mat3x3r_assigner(p, p + 1, p + 2, p + 3, p + 4, p + 5, p + 6, p + 7, p + 8); }

    const mat3x3r Orientation() const
    { real *p = _arr_orient + _ID * 9; return mat3x3r(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]); }

  private:
    const ICellObjectProvider *_objects;

    uint32_t _ID;
    size_t _mtsPerPole;
    real *_arr_pos;
    real *_arr_orient;
    int32_t *_arr_bound_mts;
};
