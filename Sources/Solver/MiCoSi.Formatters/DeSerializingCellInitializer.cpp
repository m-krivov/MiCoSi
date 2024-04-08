#include "DeSerializingCellInitializer.h"

#include "MiCoSi.Objects/All.h"

//------------------------------------
//--- DeSerializingCellInitializer ---
//------------------------------------

DeSerializingCellInitializer::DeSerializingCellInitializer(size_t chrPairs, size_t mtsPerPole)
{
  _chrPairs = chrPairs;
  _mtsPerPole = mtsPerPole;
}

void DeSerializingCellInitializer::GetCellConfig(size_t &chrPairs, size_t &mtsPerPole)
{
  chrPairs = _chrPairs;
  mtsPerPole = _mtsPerPole;
}

void DeSerializingCellInitializer::InitializeCell(ICell *cell, Random::State &state)
{
  // Set flags
  cell->SetSpringFlag(false);

  // Set poles
  cell->GetPole(PoleType::Left)->Position() = vec3r::ZERO;
  cell->GetPole(PoleType::Right)->Position() = vec3r::ZERO;

  // Set tubes
  const std::vector<MT *> &MTs = cell->MTs();
  for (size_t i = 0; i < MTs.size(); i++)
  {
    MTs[i]->Direction() = vec3r::DEFAULT_DIRECT;
    MTs[i]->Length() = (real)0.0;
    MTs[i]->State() = MTState::Polymerization;
    if (MTs[i]->BoundChromosome() != 0)
    { MTs[i]->UnBind(); }
  }

  // Set chromosomes
  const std::vector<Chromosome *> &chrs = cell->Chromosomes();
  for (size_t i = 0; i < chrs.size(); i++)
  {
    chrs[i]->Position() = vec3r::ZERO;
    chrs[i]->Orientation() = mat3x3r::IDENTITY;
  }
}
