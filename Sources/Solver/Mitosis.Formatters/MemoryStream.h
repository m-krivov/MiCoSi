
#pragma once

#include "Mitosis.Core/Defs.h"

class MemoryStream
{
  private:
    void* _data;
    size_t _capacity;
    size_t _length;
    size_t _addition;
    size_t _position;
  public:
    MemoryStream(size_t sizeInBytes = 1024);

    inline size_t Capacity()
    { return _capacity; }

    inline size_t Length()
    { return _length; }

    inline size_t Position()
    { return _position; }

    inline void Reset()
    { _position = _length = 0; }

    inline void* GetBuffer()
    { return _data; }

    void Write(void *data, size_t sizeInBytes);
    void Write(void *data, size_t offset, size_t sizeInBytes);
    void Write(MemoryStream* ms);

    ~MemoryStream();
};
