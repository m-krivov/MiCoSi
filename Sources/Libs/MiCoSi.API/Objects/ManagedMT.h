#pragma once

#include "MiCoSi.Objects/MT.h"
#include "MiCoSi.Objects/Chromosome.h"
#include "../Geometry/Converter.h"
#include "IObjectWithID.h"
#include "MappedObjects.h"
#include "ManagedPole.h"

namespace MiCoSi
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
        MiCoSi::Pole ^get()
        { return _objects->GetPole((System::UInt32)_mt->GetPole()->Type()); }
      }

      property Vector3 Direction
      {
        Vector3 get()
        { return Converter::ToVector3((vec3r)_mt->Direction()); }
      }

      property Vector3 ForceOffset
      {
        Vector3 get()
        { return Converter::ToVector3((vec3r)_mt->ForceOffset()); }
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

      property Vector3 EndPoint
      {
        Vector3 get()
        { return Converter::ToVector3((vec3r)_mt->EndPoint()); }
      }

      property Vector3 ForcePoint
      {
        Vector3 get()
        { return Converter::ToVector3((vec3r)_mt->ForcePoint()); }
      }
  };
}
