
#pragma once

#include "Mitosis.Objects/MT.h"
#include "Mitosis.Objects/Chromosome.h"
#include "../Geometry/ManagedVec3D.h"
#include "IObjectWithID.h"
#include "MappedObjects.h"
#include "ManagedPole.h"

namespace Mitosis
{
  public enum class MTState
  {
    Polymerization    = ::MTState::Polymerization,
    Depolymerization  = ::MTState::Depolymerization
  };

  public ref class MT : public IObjectWithID
  {
    private:
      ::MT *_mt;
      MappedObjects ^_objects;

    internal:
      MT(::MT *mt, MappedObjects ^objects)
      {
        _mt = mt;
        _objects = objects;
      }

      ::MT *GetObject()
      { return _mt; }

    public:
      property System::UInt32 ID
      {
        virtual System::UInt32 get()
        { return _mt->ID(); }
      }

      virtual bool Equals(System::Object ^obj) override
      {
        MT ^mt = dynamic_cast<MT ^>(obj);
        return mt != nullptr && mt->GetObject() == GetObject();
      }

      property Pole ^Pole
      {
        Mitosis::Pole ^get()
        { return _objects->GetPole((System::UInt32)_mt->GetPole()->Type()); }
      }

      property Vec3D ^Direction
      {
        Vec3D ^get()
        { return gcnew Vec3D((vec3r)_mt->Direction()); }
      }

      property Vec3D ^ForceOffset
      {
        Vec3D ^get()
        { return gcnew Vec3D((vec3r)_mt->ForceOffset()); }
      }

      property double Length
      {
        double get()
        { return (double)_mt->Length(); }
      }

      property MTState State
      {
        MTState get()
        { return _mt->State() == ::MTState::Polymerization
              ? MTState::Polymerization
              : MTState::Depolymerization; }
      }

      property Chromosome ^BoundChromosome
      {
        Chromosome ^get()
        {
          if (_mt->BoundChromosome() == NULL)
            return nullptr;
          else
            return _objects->GetChromosome(_mt->BoundChromosome()->ID());
        }
      }

      property Vec3D ^EndPoint
      {
        Vec3D ^get()
        { return gcnew Vec3D((vec3r)_mt->EndPoint()); }
      }

      property Vec3D ^ForcePoint
      {
        Vec3D ^get()
        { return gcnew Vec3D((vec3r)_mt->ForcePoint()); }
      }
  };
}
