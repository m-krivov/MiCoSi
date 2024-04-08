#include "MemoryStream.h"

//--------------------
//--- MemoryStream ---
//--------------------

MemoryStream::MemoryStream(size_t sizeInBytes):
  _position(0), _length(0), _addition(sizeInBytes), _capacity(sizeInBytes)
{
  _data = malloc(sizeInBytes);
}

void MemoryStream::Write(void *data, size_t sizeInBytes)
{
  Write(data, _length, sizeInBytes);
}

void MemoryStream::Write(void *data, size_t offset, size_t sizeInBytes)
{
  if(offset >= _capacity)
  { _capacity = offset; }

  if(offset + sizeInBytes >= _capacity)
  {
    _capacity += sizeInBytes + _addition;
    _addition = _addition > sizeInBytes ? _addition : sizeInBytes;
    void *res = realloc(_data, _capacity);
    if(!res)
    { throw std::runtime_error("MemoryStream::Write() - cannot realloc memory"); }
    _data = res;
  }

  memcpy((uint8_t *)_data + offset, data, sizeInBytes);
  _position = offset + sizeInBytes;
  _length = _position > _length ? _position : _length;
}

void MemoryStream::Write(MemoryStream* ms)
{
  Write(ms->GetBuffer(), ms->Length());
}

MemoryStream::~MemoryStream()
{
  if(_data != nullptr)
  { free(_data); }
}
