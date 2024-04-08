#pragma once

#include "MiCoSi.Objects/Cell.h"
#include "ManagedCellData.h"
#include "ManagedPole.h"
#include "ManagedMT.h"
#include "ManagedChromosome.h"
#include "ManagedChromosomePair.h"

using namespace System::Collections::Generic;

namespace MiCoSi
{
  public ref class Cell : public IObjectWithID
  {
    private:
      const ::Cell *_cell;
      CellData ^_data;
      MappedObjects ^_objects;
      Dictionary<PoleType, Pole ^> ^_poles;
      List<MT ^> ^_mts;
      List<Chromosome ^> ^_chromosomes;
      List<ChromosomePair ^> ^_pairs;

    internal:
      Cell(const ::Cell *cell)
      {
        // Creating data
        _data = gcnew CellData(&cell->Data());

        // Creating objects
        _cell = cell;
        _objects = gcnew MappedObjects();
        _poles = gcnew Dictionary<PoleType, Pole ^>();
        _mts = gcnew List<MT ^>();
        _chromosomes = gcnew List<Chromosome ^>();
        _pairs = gcnew List<ChromosomePair ^>();

        // Creating wrappers
        _poles[PoleType::Left] = gcnew Pole(cell->GetPole(::PoleType::Left));
        _objects->MapPole((System::UInt32)PoleType::Left, _poles[PoleType::Left]);
        _poles[PoleType::Right] = gcnew Pole(cell->GetPole(::PoleType::Right));
        _objects->MapPole((System::UInt32)PoleType::Right, _poles[PoleType::Right]);

        for (size_t i = 0; i < cell->MTs().size(); i++)
        {
          ::MT *ummt = cell->MTs()[i];
          MT ^mmt = gcnew MT(ummt, _objects);
          _mts->Add(mmt);
          _objects->MapMT(ummt->ID(), mmt);
        }

        for (size_t i = 0; i < cell->Chromosomes().size(); i++)
        {
          ::Chromosome *umchr = cell->Chromosomes()[i];
          Chromosome ^mchr = gcnew Chromosome(cell, umchr, _objects);
          _chromosomes->Add(mchr);
          _objects->MapChromosome(umchr->ID(), mchr);
        }

        for (size_t i = 0; i < cell->ChromosomePairs().size(); i++)
        {
          ::ChromosomePair *ump = cell->ChromosomePairs()[i];
          auto mp = gcnew MiCoSi::ChromosomePair(ump, _objects);
          _pairs->Add(mp);
          _objects->MapChromosomePair(ump->ID(), mp);
          if (mp->Spring != nullptr)
            _objects->MapSpring(ump->GetSpring()->GetID(), mp->Spring);
        }
      }      

      const ::Cell *GetObject()
      { return _cell; }

    public:
      property System::UInt32 ID
      {
        virtual System::UInt32 get()
        { return 0; }
      }

      virtual bool Equals(System::Object ^obj) override
      {
        Cell ^cell = dynamic_cast<Cell ^>(obj);
        return cell != nullptr && cell->GetObject() == GetObject();
      }

      property bool AreSpringsBroken
      {
        bool get()
        { return _cell->AreSpringsBroken(); }
      }

      Pole ^GetPole(PoleType type)
      { return _poles[type]; }

      property IEnumerable<MT ^> ^MTs
      {
        IEnumerable<MT ^> ^get()
        { return _mts; }
      }

      property IEnumerable<Chromosome ^> ^Chromosomes
      {
        IEnumerable<Chromosome ^> ^get()
        { return _chromosomes; }
      }

      property IEnumerable<ChromosomePair ^> ^ChromosomePairs
      {
        IEnumerable<ChromosomePair ^> ^get()
        { return _pairs; }
      }

      property CellData ^Data
      {
        CellData ^get()
        { return _data; }
      }
  };
}
