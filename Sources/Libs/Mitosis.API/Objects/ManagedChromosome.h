
#pragma once

#include "Mitosis.Objects/MT.h"
#include "Mitosis.Objects/Chromosome.h"
#include "Mitosis.Objects/CellOps.h"
#include "../Geometry/Matrix3x3.h"
#include "IObjectWithID.h"
#include "MappedObjects.h"

using namespace System::Collections::Generic;

namespace Mitosis
{
  public ref class Chromosome : public IObjectWithID
  {
    private:
      const ::Cell *_cell;
      const ::Chromosome *_chr;
      MappedObjects ^_objects;

    internal:
      Chromosome(const ::Cell *cell, const ::Chromosome *chr, MappedObjects ^objects)
      {
        _cell = cell;
        _chr = chr;
        _objects = objects;
      }

      const ::Chromosome *GetObject()
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

      property Vector3 Position
      {
        Vector3 get()
        { return Converter::ToVector3((vec3r)_chr->Position()); }
      }

      property Matrix3x3 Orientation
      {
        Matrix3x3 get()
        { return Matrix3x3((mat3x3r)_chr->Orientation()); }
      }

      property IEnumerable<MT ^> ^BoundMTs
      {
        IEnumerable<MT ^> ^get()
        {
          CellOps ops(_cell);
          auto bound_mts = ops.ExtractKMTs(_chr);
          auto res = gcnew List<MT ^>();
          for (size_t i = 0; i < bound_mts.size(); i++)
          { res->Add(_objects->GetMT(bound_mts[i]->ID())); }
          return res;
        }
      }
  };
}
