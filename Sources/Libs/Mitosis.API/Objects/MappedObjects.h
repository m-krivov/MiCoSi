
#pragma once

using namespace System::Collections::Generic;

namespace Mitosis
{
  generic <class T>
  private ref class OneTypeMappedObjects
  {
    private:
      Dictionary<System::UInt32, T> ^_objects;

    public:
      OneTypeMappedObjects()
      { _objects = gcnew Dictionary<System::UInt32, T>(); }

      T Get(System::UInt32 keyObject)
      { return safe_cast<T>(_objects[keyObject]); }

      void Map(System::UInt32 keyObject, T valueObject)
      { _objects[keyObject] = valueObject; }

      void UnMapAll()
      { _objects->Clear(); }
  };

  ref class Pole;
  ref class MT;
  ref class Chromosome;
  ref class ChromosomePair;
  ref class Spring;

  ///<summary>
  /// Auxiliary class that maps unmanaged objects to managed analogues.
  ///</summary>
  private ref class MappedObjects
  {
    private:
      OneTypeMappedObjects<Pole ^> ^_poles;
      OneTypeMappedObjects<MT ^> ^_mts;
      OneTypeMappedObjects<Chromosome ^> ^_chromosomes;
      OneTypeMappedObjects<ChromosomePair ^> ^_pairs;
      OneTypeMappedObjects<Spring ^> ^_springs;

    internal:
      MappedObjects()
      {
        _poles = gcnew OneTypeMappedObjects<Pole ^>();
        _mts = gcnew OneTypeMappedObjects<MT ^>();
        _chromosomes = gcnew OneTypeMappedObjects<Chromosome ^>();
        _pairs = gcnew OneTypeMappedObjects<ChromosomePair ^>();
        _springs = gcnew OneTypeMappedObjects<Spring ^>();
      }

    public:
      Pole ^GetPole(System::UInt32 type)
      { return _poles->Get(type); }

      void MapPole(System::UInt32 pole, Pole ^mpole)
      { _poles->Map(pole, mpole); }

      MT ^GetMT(System::UInt32 mt)
      { return _mts->Get(mt); }

      void MapMT(System::UInt32 mt, MT ^mmt)
      { _mts->Map(mt, mmt); }

      Chromosome ^GetChromosome(System::UInt32 chr)
      { return _chromosomes->Get(chr); }

      void MapChromosome(System::UInt32 chr, Chromosome ^mchr)
      { _chromosomes->Map(chr, mchr); }

      ChromosomePair ^GetChromosomePair(System::UInt32 pair)
      { return _pairs->Get(pair); }

      void MapChromosomePair(System::UInt32 pair, ChromosomePair ^mpair)
      { _pairs->Map(pair, mpair); }

      Spring ^GetSpring(System::UInt32 spring)
      { return _springs->Get(spring); }

      void MapSpring(System::UInt32 spring, Spring ^mspring)
      { _springs->Map(spring, mspring); }
  };  
}
