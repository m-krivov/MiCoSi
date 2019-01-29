
#pragma once

#include "ChunkHeader.h"

class FileContainer;

class Chunk
{
  private:
    ChunkHeader _header;
    std::unique_ptr<uint8_t[]> _metaData;
    std::unique_ptr<uint8_t[]> _binData;

  private:
    Chunk(const ChunkHeader &header)
      : _header(header)
    {
      _metaData.reset(new uint8_t[(size_t)Header().MetaDataSize()]);
      _binData.reset(new uint8_t[(size_t)Header().BinDataSize()]);
    }

  public:
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

  friend class FileContainer;
};
