#pragma once
#include "MiCoSi.Core/Defs.h"

#include "Interfaces.h"
#include "CellData.h"

// Enumeration with MT states
class MTState
{
  public:
    enum Type : uint32_t
    {
      Polymerization    = 0,
      Depolymerization  = 1
    };
};

// Describes one MicroTubule
class MT
{
  public:
    MT(const MT &) = delete;
    MT(uint32_t ID, const ICellObjectProvider *objects, const CellData *data);
    MT &operator =(const MT &) = delete;

    // Basic properties

    uint32_t ID() const
    { return _ID; }

    vec3r Direction() const
    { return vec3r(_arr_dir_x[_ID], _arr_dir_y[_ID], _arr_dir_z[_ID]); }

    vec3r_assigner Direction()
    { return vec3r_assigner(_arr_dir_x + _ID, _arr_dir_y + _ID, _arr_dir_z + _ID); }

    real &Length()
    { return _arr_lengthes[_ID]; }

    const real &Length() const
    { return _arr_lengthes[_ID]; }

    vec3r ForceOffset() const
    { return vec3r(_arr_force_x[_ID], _arr_force_y[_ID], _arr_force_z[_ID]); }

    vec3r_assigner ForceOffset()
    { return vec3r_assigner(_arr_force_x + _ID, _arr_force_y + _ID, _arr_force_z + _ID); }

    MTState::Type State() const
    { return (MTState::Type)_arr_states[_ID]; }

    MTState::Type &State()
    { return ((MTState::Type *)_arr_states)[_ID]; }

    // References to other objects

    const Pole *GetPole() const
    { return _objects->GetPole(_arr_poles[_ID]); }

    // The 'nullptr' value means that MT is not bound
    Chromosome *BoundChromosome();

    void Bind(Chromosome *cr);

    // Defines the point which the MT tail is located in
    vec3r EndPoint() const
    { return GetPole()->Position() + Direction() * Length(); }

    // Defines the point which force will be applied to
    vec3r ForcePoint() const
    { return EndPoint() + ForceOffset(); }

    void UnBind()
    { _arr_bound_chrs[_ID] = -1; }

  private:
    const ICellObjectProvider *_objects;

    uint32_t _ID;
    uint32_t *_arr_poles;
    real *_arr_dir_x, *_arr_dir_y, *_arr_dir_z;
    real *_arr_force_x, *_arr_force_y, *_arr_force_z;
    real *_arr_lengthes;
    uint32_t *_arr_states;
    int32_t *_arr_bound_chrs;
};
