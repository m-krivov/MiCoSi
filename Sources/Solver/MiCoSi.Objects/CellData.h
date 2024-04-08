#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/Interfaces.h"

// Indices for the offset table, can be used from CUDA
class CellArray
{
  public:
    CellArray() = delete;
    CellArray(const CellArray &) = delete;
    CellArray &operator =(const CellArray &) = delete;

    enum Type
    {
      POLE_POSITION             = 0,   // real * 3
      POLE_TYPE                 = 1,   // uint32
      MT_POLE                   = 2,   // uint32
      MT_DIRECTION_X            = 3,   // real
      MT_DIRECTION_Y            = 4,   // real
      MT_DIRECTION_Z            = 5,   // real
      MT_FORCE_OFFSET_X         = 6,   // real
      MT_FORCE_OFFSET_Y         = 7,   // real
      MT_FORCE_OFFSET_Z         = 8,   // real
      MT_LENGTH                 = 9,   // real
      MT_STATE                  = 10,  // uint32
      MT_BOUND_CHROMOSOME       = 11,  // int32, can be negative
      CHR_POSITION              = 12,  // real * 3
      CHR_ORIENTATION           = 13,  // real * 9
      CHR_PAIR_LEFF_CHROMOSOME  = 14,  // uint32
      CHR_PAIR_RIGHT_CHROMOSOME = 15,  // uint32
      SPRINGS_BROKEN            = 16   // single uint32
    };

    // Returns size (in bytes) of the required array
    static size_t GetSize(Type type, size_t chrPairs, size_t mtsPerPole);

    // Returns enumeration of all array types
    static std::vector<Type> AllTypes();
};

// Container for interleaved values
// Stores all cell data into single chunk and provides offsets for each array type
class CellData : public IClonnable
{
  public:
    // Creates data for cell with required number of chromosomes and MTs
    CellData(size_t chrPairs, size_t mtsPerPole);

    CellData(const CellData &) = delete;
    CellData operator =(const CellData &) = delete;

    // Returns count of chromosome pairs, for which cell data was created
    size_t ChromosomePairs() const
    { return _chrPairs; }

    // Returns count of MTs, for which cell data was created
    size_t MTsPerPole() const
    { return _mtsPerPole; }

    // Returns pointer for block with all values. Each array is aligned
    uint8_t *DataPointer() const
    { return _data; }

    // Size of values, in bytes
    size_t DataSize() const
    { return _dataSize; }

    // Returns pointer for offset table
    // E.g. the "DataPointer() + OffsetPointer()[CellData::MT_LENGTH]" expression can be used for
    // getting array with lengthes of MTs
    uint64_t *OffsetPointer() const
    { return _offsets; }

    // Size of offset table, in bytes
    size_t OffsetSize() const
    { return _offsetSize; }

    // Returns pointer to required array
    void *GetArray(CellArray::Type type) const
    { return _data + _offsets[type]; }

    // IClonnable member
    virtual IClonnable *Clone() const override;

    virtual ~CellData();

  private:
    static const int ALIGNMENT = 64;  // in bytes
    
    uint8_t *_data;
    uint64_t *_offsets;
    size_t _dataSize, _offsetSize;
    size_t _chrPairs, _mtsPerPole;
};
