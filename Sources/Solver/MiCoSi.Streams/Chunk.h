#pragma once
#include "MiCoSi.Core/Defs.h"

#include "ChunkHeader.h"

class FileContainer;

class Chunk
{
  public:
    Chunk() = delete;
    Chunk(const Chunk &) = delete;
    Chunk &operator =(const Chunk &) = delete;

    // All information about this chunk.
    const ChunkHeader &Header() const
    { return _header; }

    // Non-parsed meta data. Size can acquired from Header.
    void *MetaDataPointer() const
    { return _metaData.get(); }

    // Non-parsed binary data. Size can acquired from Header.
    void *BinDataPointer() const
    { return _binData.get(); }

    ~Chunk() { /*nothing*/ }

  private:
    Chunk(const ChunkHeader &header)
      : _header(header)
    {
      _metaData.reset(new uint8_t[(size_t)Header().MetaDataSize()]);
      _binData.reset(new uint8_t[(size_t)Header().BinDataSize()]);
    }

    ChunkHeader _header;
    std::unique_ptr<uint8_t[]> _metaData;
    std::unique_ptr<uint8_t[]> _binData;

  friend class FileContainer;
};
