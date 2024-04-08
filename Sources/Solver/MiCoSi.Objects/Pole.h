#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Geometry/vec3x.h"
#include "CellData.h"

// Enumeration for pole types
enum class PoleType : uint32_t
{
  Left  = 0,
  Right = 1
};

// Describes one spindle pole of the cell
class Pole
{
  public:
    Pole(uint32_t ID, const CellData *data);
    Pole(const Pole &) = delete;
    Pole &operator =(const Pole &) = delete;

    uint32_t ID() const
    { return _ID; }

    vec3r_assigner Position()
    { real *p = _arr_positions + 3 * _ID; return vec3r_assigner(p, p + 1, p + 2); }

    const vec3r Position() const
    { real *p = _arr_positions + 3 * _ID; return vec3r(p[0], p[1], p[2]); }

    PoleType Type() const
    { return (PoleType)_arr_types[_ID]; }

  private:
    void SetType(PoleType &type)
    { ((PoleType *)_arr_types)[_ID] = type; }
    
    uint32_t _ID;
    real *_arr_positions;
    uint32_t *_arr_types;

  friend class IPoleUpdater;
};
