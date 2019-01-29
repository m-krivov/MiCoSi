
#pragma once

#include "Mitosis.Objects/MT.h"
#include "Mitosis.Objects/Chromosome.h"
#include "../Geometry/ManagedVec3D.h"
#include "../Geometry/ManagedMat3x3D.h"
#include "IObjectWithID.h"
#include "MappedObjects.h"

using namespace System::Collections::Generic;

namespace Mitosis
{
  public ref class Chromosome : public IObjectWithID
  {
    private:
      ::Chromosome *_chr;
      MappedObjects ^_objects;
      List<MT ^> ^_cachedMTs;

    internal:
      Chromosome(::Chromosome *chr, MappedObjects ^objects)
      {
        _chr = chr;
        _objects = objects;
        _cachedMTs = nullptr;
      }

      ::Chromosome *GetObject()
      { return _chr; }

    public:
      property System::UInt32 ID
      {
        virtual System::UInt32 get()
        { return _chr->ID(); }
      }

      virtual bool Equals(System::Object ^obj) override
      {
        Chromosome ^chr = dynamic_cast<Chromosome ^>(obj);
        return chr != nullptr && chr->GetObject() == GetObject();
      }

      property Vec3D ^Position
      {
        Vec3D ^get()
        { return gcnew Vec3D((vec3r)_chr->Position()); }
      }

      property Mat3x3D ^Orientation
      {
        Mat3x3D ^get()
        { return gcnew Mat3x3D((mat3x3r)_chr->Orientation()); }
      }

      property IEnumerable<MT ^> ^BoundMTs
      {
        IEnumerable<MT ^> ^get()
        {
          //Lazy initialization, because MTs can be changed after chromosome creation.
          //if (_cachedMTs == nullptr)
          {
            _cachedMTs = gcnew List<MT ^>();
            std::vector<::MT *> boundMTs = _chr->BoundMTs();
            for (size_t i = 0; i < boundMTs.size(); i++)
            { _cachedMTs->Add(_objects->GetMT(boundMTs[i]->ID())); }
          }
          return _cachedMTs;
        }
      }
  };
}
