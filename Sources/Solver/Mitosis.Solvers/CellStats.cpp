
#include "CellStats.h"

#include "Mitosis.Objects/All.h"

//-----------------
//--- CellStats ---
//-----------------

CellStats CellStats::Create(size_t mtsPerPole, size_t chrPairs, bool springsBroken,
              size_t boundMTs, size_t freeMTs, size_t polyMTs, size_t depolyMTs,
              size_t minBoundMTsPerChr, size_t maxBoundMTsPerChr)
{
  return CellStats(1, mtsPerPole, chrPairs, SpringStats(springsBroken),
           MTStats((double)boundMTs, (double)freeMTs, (double)polyMTs, (double)depolyMTs),
           ChromosomeStats(minBoundMTsPerChr, maxBoundMTsPerChr));
}

CellStats CellStats::Create(const Cell *cell)
{
  size_t polyMTs = 0;
  size_t depolyMTs = 0;
  size_t boundMTs = 0;
  size_t freeMTs = 0;

  for (size_t i = 0; i < cell->MTs().size(); i++)
  {
    MT *mt = cell->MTs()[i];
    boundMTs += mt->BoundChromosome() != NULL ? 1 : 0;
    freeMTs += mt->BoundChromosome() == NULL ? 1 : 0;
    depolyMTs += mt->State() == MTState::Depolymerization ? 1 : 0;
    polyMTs += mt->State() == MTState::Polymerization ? 1 : 0;
  }

  CellOps ops(cell);
  auto kmts_per_chr = ops.CountKMTs();
  int maxBoundMTsPerChr = -1;
  int minBoundMTsPerChr = -1;

  for (size_t i = 0; i < kmts_per_chr.size(); i++)
  {
    int boundMTs = (int)kmts_per_chr[i];
    maxBoundMTsPerChr = std::max(maxBoundMTsPerChr, boundMTs);
    if (minBoundMTsPerChr < 0)
      minBoundMTsPerChr = boundMTs;
    else
      minBoundMTsPerChr = std::min(minBoundMTsPerChr, boundMTs);
  }

  return Create(cell->MTs().size() / 2, cell->ChromosomePairs().size(), cell->AreSpringsBroken(),
          boundMTs, freeMTs, polyMTs, depolyMTs, minBoundMTsPerChr, maxBoundMTsPerChr);
}

CellStats CellStats::Aggregate(const std::vector<CellStats> &stats)
{
  if (stats.size() == 0)
    throw std::runtime_error("Internal error at CellStats::Aggregate() - at least one cell's stats must be provided");
  size_t mtsPerPole = stats[0].MTsPerPole();
  size_t chrPairs = stats[0].ChromosomePairs();

  bool springsBroken = true;
  double bound = 0.0, free = 0.0, poly = 0.0, depoly = 0.0;
  size_t minBound = stats[0].Chromosomes().MinBoundMTsPerChr(),
       maxBound = stats[0].Chromosomes().MaxBoundMTsPerChr();

  for (size_t i = 0; i < stats.size(); i++)
  {
    if (stats[i].MTsPerPole() != mtsPerPole || stats[i].ChromosomePairs() != chrPairs)
      throw std::runtime_error("Internal error at CellStats::Aggregate() - cannot aggregate cells with diferent configuration");
    
    CellStats cur = stats[i];
    springsBroken &= cur.Springs().Broken();
    bound += cur.MTs().Bound();
    free += cur.MTs().Free();
    poly += cur.MTs().Polymerizing();
    depoly += cur.MTs().DePolymerizing();
    minBound = std::min(minBound, cur.Chromosomes().MinBoundMTsPerChr());
    maxBound = std::max(maxBound, cur.Chromosomes().MaxBoundMTsPerChr());
  }

  size_t cellCount = stats.size();
  return CellStats(cellCount, mtsPerPole, chrPairs, SpringStats(springsBroken),
           MTStats(bound / cellCount, free / cellCount, poly / cellCount, depoly / cellCount),
           ChromosomeStats(minBound, maxBound));
}
