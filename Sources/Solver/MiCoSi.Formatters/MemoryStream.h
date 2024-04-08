#pragma once
#include "MiCoSi.Core/Defs.h"

class MemoryStream
{
  public:
    MemoryStream(size_t sizeInBytes = 1024);

    size_t Capacity() const
    { return _capacity; }

    size_t Length() const
    { return _length; }

    size_t Position() const
    { return _position; }

    void Reset()
    { _position = _length = 0; }

    void *GetBuffer()
    { return _data; }

    void Write(void *data, size_t sizeInBytes);
    void Write(void *data, size_t offset, size_t sizeInBytes);
    void Write(MemoryStream *ms);

    ~MemoryStream();

  private:
    void *_data;
    size_t _capacity;
    size_t _length;
    size_t _addition;
    size_t _position;
};
