
#pragma once

#include "Mitosis.Core/Defs.h"

// Enumeration with chunk type.
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

// Information about some chunk. Without real data, just information.
class ChunkHeader
{
  private:
    ChunkType::Type _type;
    double _time;
    offset_t _chunkOffset;
    uint64_t _chunkSize;
    size_t _count;
    uint64_t _metaDataSize;
    uint64_t _binDataSize;

  private:
    ChunkHeader(ChunkType::Type type, double time, size_t count,
          offset_t chunkOffset, uint64_t chunkSize,
          uint64_t metaDataSize, uint64_t binDataSize)
        : _type(type), _time(time), _count(count),
        _chunkOffset(chunkOffset), _chunkSize(chunkSize),
        _metaDataSize(metaDataSize), _binDataSize(binDataSize)
    { /*nothing*/ }

  public:
    // Service or frame data?
    inline ChunkType::Type Type() const
    { return _type; }

    // Returns time of the first chunk's frame. Or 0.0 for service chunks.
    inline double StartTime() const
    { return _time; }

    // Chunk's start position, in bytes.
    inline offset_t ChunkOffset() const
    { return _chunkOffset; }

    // Returns total chunk size (including service data).
    inline uint64_t ChunkSize() const
    { return _chunkSize; }

    // Returns count of elements in chunk
    inline size_t Count() const
    { return _count; }

    // Size of meta data record, in bytes.
    inline uint64_t MetaDataSize() const
    { return _metaDataSize; }

    // Size of binary data record, in bytes.
    inline uint64_t BinDataSize() const
    { return _binDataSize; }

  friend class FileContainer;
};
