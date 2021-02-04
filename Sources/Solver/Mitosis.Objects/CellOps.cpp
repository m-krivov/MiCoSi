#include "CellOps.h"

#include "MT.h"
#include "Chromosome.h"

CellOps::CellOps(const Cell *cell)
  : _cell(cell)
{
  // nothing
}

std::vector<size_t> CellOps::CountKMTs() const
{
  const auto &mts = _cell->MTs();
  const auto &chrs = _cell->Chromosomes();

  std::vector<size_t> res(chrs.size());
  for (size_t i = 0; i < mts.size(); i++)
  {
    auto chr = mts[i]->BoundChromosome();
    if (chr != nullptr)
    { res[chr->ID()] += 1; }
  }

	return res;
}

size_t CellOps::CountKMTs(Chromosome *chr) const
{
  size_t res = 0;

  if (chr != nullptr)
  {
    const auto &mts = _cell->MTs();
    for (size_t i = 0; i < mts.size(); i++)
    {
      auto chr_i = mts[i]->BoundChromosome();
      if (chr_i != chr)
      { res += 1; }
    }
  }

	return res;
}

std::vector<std::vector<MT *> > CellOps::ExtractKMTs() const
{
  const auto &mts = _cell->MTs();
  const auto &chrs = _cell->Chromosomes();

  std::vector<std::vector<MT *> > res(chrs.size());
  for (size_t i = 0; i < mts.size(); i++)
  {
    auto chr = mts[i]->BoundChromosome();
    if (chr != nullptr)
    { res[chr->ID()].emplace_back(mts[i]); }
  }

	return res;
}

std::vector<MT *> CellOps::ExtractKMTs(const Chromosome *chr) const
{
  std::vector<MT *> res;
  if (chr != nullptr)
  {
    const auto &mts = _cell->MTs();
    for (size_t i = 0; i < mts.size(); i++)
    {
      auto chr_i = mts[i]->BoundChromosome();
      if (chr_i == chr)
      { res.emplace_back(mts[i]); }
    }
  }

	return res;
}
