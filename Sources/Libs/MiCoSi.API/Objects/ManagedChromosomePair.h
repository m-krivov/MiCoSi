#pragma once

#include "MiCoSi.Objects/ChromosomePair.h"
#include "ManagedChromosome.h"
#include "ManagedSpring.h"
#include "IObjectWithID.h"
#include "MappedObjects.h"

namespace MiCoSi
{
  public ref class ChromosomePair : public IObjectWithID
  {
    private:
      ::ChromosomePair *_pair;
      Spring ^_spring;
      MappedObjects ^_objects;

    internal:
      ChromosomePair(::ChromosomePair *pair, MappedObjects ^objects)
      {
        _pair = pair;
        _objects = objects;
        _spring = pair->GetSpring() == NULL ? nullptr : gcnew MiCoSi::Spring(pair->GetSpring());
      }

      ::ChromosomePair *GetObject()
      { return _pair; }

    public:
      property System::UInt32 ID
      {
        virtual System::UInt32 get()
        { return _pair->ID(); }
      }

      virtual bool Equals(System::Object ^obj) override
      {
        ChromosomePair ^pair = dynamic_cast<ChromosomePair ^>(obj);
        return pair != nullptr && pair->GetObject() == GetObject();
      }

      property Chromosome ^LeftChromosome
      {
        Chromosome ^get()
        { return _objects->GetChromosome(_pair->LeftChromosome()->ID()); }
      }

      property Chromosome ^RightChromosome
      {
        Chromosome ^get()
        { return _objects->GetChromosome(_pair->RightChromosome()->ID()); }
      }

      property Spring ^Spring
      {
        MiCoSi::Spring ^get()
        { return _spring; }
      }
  };
}
