#include "CellData.h"

//-----------------
//--- CellArray ---
//-----------------

size_t CellArray::GetSize(CellArray::Type type, size_t chrPairs, size_t mtsPerPole)
{
  switch (type)
  {
    case POLE_POSITION:              return sizeof(real) * 3 * 2;
    case POLE_TYPE:                  return sizeof(uint32_t) * 2;
    case MT_POLE:                    return sizeof(uint32_t) * mtsPerPole * 2;
    case MT_DIRECTION_X:             return sizeof(real) * mtsPerPole * 2;
    case MT_DIRECTION_Y:             return sizeof(real) * mtsPerPole * 2;
    case MT_DIRECTION_Z:             return sizeof(real) * mtsPerPole * 2;
    case MT_FORCE_OFFSET_X:          return sizeof(real) * mtsPerPole * 2;
    case MT_FORCE_OFFSET_Y:          return sizeof(real) * mtsPerPole * 2;
    case MT_FORCE_OFFSET_Z:          return sizeof(real) * mtsPerPole * 2;
    case MT_LENGTH:                  return sizeof(real) * mtsPerPole * 2;
    case MT_STATE:                   return sizeof(uint32_t) * mtsPerPole * 2;
    case MT_BOUND_CHROMOSOME:        return sizeof(int32_t) * mtsPerPole * 2;
    case CHR_POSITION:               return sizeof(real) * 3 * chrPairs * 2;
    case CHR_ORIENTATION:            return sizeof(real) * 9 * chrPairs * 2;
    case CHR_PAIR_LEFF_CHROMOSOME:   return sizeof(uint32_t) * chrPairs;
    case CHR_PAIR_RIGHT_CHROMOSOME:  return sizeof(uint32_t) * chrPairs;
    case SPRINGS_BROKEN:             return sizeof(uint32_t);
    default: throw std::runtime_error("Internal error at CellArray::GetSize(): unknown value");
  }
}

std::vector<CellArray::Type> CellArray::AllTypes()
{
  std::vector<CellArray::Type> res;

  res.push_back(POLE_POSITION);
  res.push_back(POLE_TYPE);
  res.push_back(MT_POLE);
  res.push_back(MT_DIRECTION_X);
  res.push_back(MT_DIRECTION_Y);
  res.push_back(MT_DIRECTION_Z);
  res.push_back(MT_FORCE_OFFSET_X);
  res.push_back(MT_FORCE_OFFSET_Y);
  res.push_back(MT_FORCE_OFFSET_Z);
  res.push_back(MT_LENGTH);
  res.push_back(MT_STATE);
  res.push_back(MT_BOUND_CHROMOSOME);
  res.push_back(CHR_POSITION);
  res.push_back(CHR_ORIENTATION);
  res.push_back(CHR_PAIR_LEFF_CHROMOSOME);
  res.push_back(CHR_PAIR_RIGHT_CHROMOSOME);
  res.push_back(SPRINGS_BROKEN);

  return res;
}

//----------------
//--- CellData ---
//----------------

CellData::CellData(size_t chrPairs, size_t mtsPerPole)
  : _chrPairs(chrPairs), _mtsPerPole(mtsPerPole)
{
  auto types = CellArray::AllTypes();

  //Allocating offset table
  _offsetSize = sizeof(uint64_t) * types.size();
  _offsets = (uint64_t *)malloc(_offsetSize);

  //Calculating data size for aligned arrays, filling offset table
  _dataSize = 0;
  for (size_t i = 0; i < types.size(); i++)
  {
    if ((uint32_t)types[i] >= types.size())
      throw std::runtime_error("Internal error at CellData::CellData() - wrong enum values");

    _offsets[types[i]] = _dataSize;
    size_t size = CellArray::GetSize(types[i], chrPairs, mtsPerPole);
    _dataSize += ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
  }

  // Allocating and clearing memory for arrays
  _data = (uint8_t *)malloc(_dataSize);
  memset(_data, 0, _dataSize);
}

IClonnable *CellData::Clone() const
{
  CellData *res = nullptr;

  try
  {
    res = new CellData(ChromosomePairs(), MTsPerPole());

    if (res->DataSize() != DataSize() ||
      res->OffsetSize() != OffsetSize())
      throw std::runtime_error("Internal error at CellData::Clone() - wrong size of allocated memory blocks");

    memcpy(res->DataPointer(), DataPointer(), DataSize());
    memcpy(res->OffsetPointer(), OffsetPointer(), OffsetSize());

    return res;
  }
  catch (std::exception &)
  {
    if (res != nullptr) delete res;
    throw;
  }
}

CellData::~CellData()
{
  if (_data != nullptr)
  {
    free(_data);
    _data = nullptr;
  }

  if (_offsets != nullptr)
  {
    free(_offsets);
    _offsets = nullptr;
  }
}
