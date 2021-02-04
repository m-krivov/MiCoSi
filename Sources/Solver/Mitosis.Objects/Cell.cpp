
#include "Cell.h"

#include "Pole.h"
#include "MT.h"
#include "Chromosome.h"
#include "ChromosomePair.h"
#include "Spring.h"

//------------
//--- Cell ---
//------------

void Cell::CreateObjects(CellData *data)
{
  // Setting pointer to spring flag.
  _springs_broken_flag = (uint32_t *)_data->GetArray(CellArray::SPRINGS_BROKEN);

  // Creating poles.
  _poles[(uint32_t)PoleType::Left] = new Pole((uint32_t)PoleType::Left, _data.get());
  _poles[(uint32_t)PoleType::Right] = new Pole((uint32_t)PoleType::Right, _data.get());

  // Creating MTs.
  if (_data->MTsPerPole() != 0)
  {
    _MTs.resize(_data->MTsPerPole() * 2, NULL);
    for (uint32_t i = 0; i < _MTs.size(); i++)
      _MTs[i] = new MT(i, this, _data.get());
  }

  // Creating chromosomes.
  if (_data->ChromosomePairs() != 0)
  {
    _chromosomes.resize(_data->ChromosomePairs() * 2, NULL);
    for (uint32_t i = 0; i < _chromosomes.size(); i++)
      _chromosomes[i] = new Chromosome(i, this, _data.get());

    _chromosomePairs.resize(_data->ChromosomePairs(), NULL);
    _springs.resize(_data->ChromosomePairs(), NULL);
    for (uint32_t i = 0; i < _chromosomePairs.size(); i++)
    {
      _chromosomePairs[i] = new ChromosomePair(i, this, _data.get());
      _springs[i] = new Spring(i, this, _data.get());
    }
  }
}

Cell::Cell(CellData *data)
{
  _data.reset(data);
  CreateObjects(_data.get());
}

Cell::Cell(ICellInitializer *initializer, IPoleUpdater *updater, Random::State &state)
{
  // Construct cell data and objects
  size_t chrPairs = 0, mtsPerPole = 0;
  initializer->GetCellConfig(chrPairs, mtsPerPole);
  _data.reset(new CellData(chrPairs, mtsPerPole));
  CreateObjects(_data.get());

  // Set poles
  uint32_t *poleType = (uint32_t *)_data->GetArray(CellArray::POLE_TYPE);
  poleType[(uint32_t)PoleType::Left]  = (uint32_t)PoleType::Left;
  poleType[(uint32_t)PoleType::Right] = (uint32_t)PoleType::Right;

  // Set MTs
  if (_data->MTsPerPole() != 0)
  {
    uint32_t *mtPole = (uint32_t *)_data->GetArray(CellArray::MT_POLE);
    int32_t *bondChr = (int32_t *)_data->GetArray(CellArray::MT_BOUND_CHROMOSOME);
    for (size_t i = 0; i < _MTs.size(); i++)
    {
      mtPole[i] = i < _data->MTsPerPole() ? (uint32_t)PoleType::Left : (uint32_t)PoleType::Right;
      bondChr[i] = -1;
    }
  }

  // Set chromosomes
  if (_data->ChromosomePairs() != 0)
  {
    uint32_t *chrPairLeft  = (uint32_t *)_data->GetArray(CellArray::CHR_PAIR_LEFF_CHROMOSOME);
    uint32_t *chrPairRight = (uint32_t *)_data->GetArray(CellArray::CHR_PAIR_RIGHT_CHROMOSOME);
    for (uint32_t i = 0; i < _chromosomePairs.size(); i++)
    {
      chrPairLeft[i] = 2 * i;
      chrPairRight[i] = 2 * i + 1;
    }
  }

  // All structures were created, indices were set
  // So, initialize values
  initializer->InitializeCell(this, state);
  updater->SetInitial(GetPole(PoleType::Left), GetPole(PoleType::Right), state);
}

IClonnable *Cell::Clone() const
{
  CellData *data = nullptr;
  try
  {
    data = _data->CloneTemplated<CellData>();
    return new Cell(data);
  }
  catch (std::exception &)
  {
    if (data != NULL) delete data;
    throw;
  }
}

#define SAFE_DELETE_CONTENT(vec, size)    \
{                                         \
  for (size_t i = 0; i < size; i++)       \
    if (vec[i] != NULL)                   \
    { delete vec[i]; vec[i] = NULL; }     \
}

#define SAFE_DELETE(vec)                  \
{                                         \
  SAFE_DELETE_CONTENT(vec, vec.size());   \
  vec.clear();                            \
}

Cell::~Cell()
{
  SAFE_DELETE_CONTENT(_poles, 2);
  SAFE_DELETE(_MTs);
  SAFE_DELETE(_chromosomes);
  SAFE_DELETE(_chromosomePairs);
  SAFE_DELETE(_springs);
}
