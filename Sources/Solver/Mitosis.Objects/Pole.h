
#pragma once

#include "Mitosis.Core/Defs.h"
#include "Mitosis.Geometry/vec3x.h"

#include "CellData.h"

// Enumeration for pole types.
class PoleType
{
  public:
    enum Type : uint32_t
    {
      Left  = 0,
      Right  = 1
    };
};

// Describes one pole in cell.
class Pole
{
  private:
    uint32_t _ID;
    real *_arr_positions;
    uint32_t *_arr_types;

  private:
    inline void SetType(PoleType::Type &type)
    { ((PoleType::Type *)_arr_types)[_ID] = type; }

    Pole(const Pole &) = delete;
    void operator =(const Pole &) = delete;

  public:
    Pole(uint32_t ID, const CellData *data);

    inline uint32_t ID() const
    { return _ID; }

    inline vec3r_assigner Position()
    { real *p = _arr_positions + 3 * _ID; return vec3r_assigner(p, p + 1, p + 2); }

    inline const vec3r Position() const
    { real *p = _arr_positions + 3 * _ID; return vec3r(p[0], p[1], p[2]); }

    inline PoleType::Type Type() const
    { return (PoleType::Type)_arr_types[_ID]; }

  friend class IPoleUpdater;
};
