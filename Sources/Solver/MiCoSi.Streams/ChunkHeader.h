#pragma once
#include "MiCoSi.Core/Defs.h"

// Enumeration with chunk types
class ChunkType
{
  public:
    enum Type
    {
      Service,    // any data for cell configuration and other not-timed information
      Frames      // set of the time layers
    };
};

class FileContainer;

// Information about some chunk
// Without real data, just an information
class ChunkHeader
{
  public:
    ChunkHeader() = delete;
    ChunkHeader(const ChunkHeader &) = default;             // TODO: avoid copy ctor
    ChunkHeader &operator =(const ChunkHeader &) = delete;

    // Service or frame data?
    ChunkType::Type Type() const
    { return _type; }

    // Returns time of the first chunk's frame. Or 0.0 for service chunks
    double StartTime() const
    { return _time; }

    // Chunk's start position, in bytes
    offset_t ChunkOffset() const
    { return _chunkOffset; }

    // Returns total chunk size (including service data)
    uint64_t ChunkSize() const
    { return _chunkSize; }

    // Returns count of elements in chunk
    size_t Count() const
    { return _count; }

    // Size of meta data record, in bytes
    uint64_t MetaDataSize() const
    { return _metaDataSize; }

    // Size of binary data record, in bytes
    uint64_t BinDataSize() const
    { return _binDataSize; }

  private:
    ChunkHeader(ChunkType::Type type, double time, size_t count,
                offset_t chunkOffset, uint64_t chunkSize,
                uint64_t metaDataSize, uint64_t binDataSize)
      : _type(type), _time(time), _count(count),
        _chunkOffset(chunkOffset), _chunkSize(chunkSize),
        _metaDataSize(metaDataSize), _binDataSize(binDataSize)
    { /*nothing*/ }

    ChunkType::Type _type;
    double _time;
    offset_t _chunkOffset;
    uint64_t _chunkSize;
    size_t _count;
    uint64_t _metaDataSize;
    uint64_t _binDataSize;

  friend class FileContainer;
};
