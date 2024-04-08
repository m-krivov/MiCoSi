#pragma once
#include "MiCoSi.Core/Defs.h"

// Definition of the used class
class Cell;

// Container with the aggregated information about cell
class CellStats
{
  public:
    class SpringStats
    {
      public:
        bool Broken() const { return _broken; }

      private:
        SpringStats(bool broken) : _broken(broken) { }
        bool _broken;

      friend class CellStats;
    };

    class MTStats
    {
      public:
        double Bound() const { return _bound; }
        double Free() const { return _free; }
        double Polymerizing() const { return _poly; }
        double DePolymerizing() const { return _depoly; }

     private:
        MTStats(double bound, double free, double poly, double depoly)
          : _bound(bound), _free(free), _poly(poly), _depoly(depoly)
        {
          if (std::abs((_bound + _free) - (_poly + _depoly)) > 1e-3)
            throw std::runtime_error("Internal error - wrong count of MTs");
        }
        double _bound, _free, _poly, _depoly;

      friend class CellStats;
    };

    class ChromosomeStats
    {
      public:
        size_t MinBoundMTsPerChr() const { return _minBound; }
        size_t MaxBoundMTsPerChr() const { return _maxBound; }

      private:
        ChromosomeStats(size_t minBound, size_t maxBound)
          : _minBound(minBound), _maxBound(maxBound) { }
        size_t _minBound, _maxBound;

      friend class CellStats;
    };

    size_t AggregatedCells() const;
    size_t MTsPerPole() const { return _mtsPerPole; }
    size_t ChromosomePairs() const { return _chrPairs; }
    const SpringStats &Springs() const { return _springs; }
    const MTStats &MTs() const { return _mts; }
    const ChromosomeStats &Chromosomes() const { return _chrs; }

    static CellStats Create(size_t mtsPerPole, size_t chrPairs, bool springsBroken,
                            size_t boundMTs, size_t freeMTs, size_t polyMTs, size_t depolyMTs,
                            size_t minBoundMTsPerChr, size_t maxBoundMTsPerChr);
    static CellStats Create(const Cell *cell);
    static CellStats Aggregate(const std::vector<CellStats> &stats);

  private:
    size_t _aggregatedCells;
    size_t _mtsPerPole;
    size_t _chrPairs;
    SpringStats _springs;
    MTStats _mts;
    ChromosomeStats _chrs;

    inline CellStats(size_t aggregatedCells, size_t mtsPerPole, size_t chrPairs,
             const SpringStats &springs, const MTStats &mts, const ChromosomeStats &chrs)
      : _aggregatedCells(aggregatedCells), _mtsPerPole(mtsPerPole), _chrPairs(chrPairs),
        _springs(springs), _mts(mts), _chrs(chrs)
    { /*nothing*/ }
};

class ICellStatsProvider
{
  public:
    virtual ~ICellStatsProvider() = default;
    virtual const std::vector<CellStats> &Stats() = 0;

  protected:
    ICellStatsProvider() = default;
};
