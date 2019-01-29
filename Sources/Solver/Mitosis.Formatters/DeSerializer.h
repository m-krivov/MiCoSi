
#pragma once

#include "Mitosis.Core/Versions.h"
#include "Mitosis.Core/SimParams.h"
#include "Mitosis.Objects/Cell.h"

#include "tinyxml.h"
#include "MemoryStream.h"


// Static class that performs deserialization of the internal objects.
class DeSerializer
{
  private:
    class Loader
    {
      private:
        void *_data;
        size_t _sizeInBytes;

      public:
        Loader(void *data, size_t sizeInBytes)
          : _data(data), _sizeInBytes(sizeInBytes)
        { /*nothing*/ }

        void LoadArray(size_t offset, size_t size, void *dst)
        {
          if (offset + size > _sizeInBytes)
            throw std::runtime_error("Cannot load array");
          memcpy(dst, (uint8_t *)_data + offset, size);
        }
    };

  private:
    template <class T>
    static void DeserializeArray(TiXmlElement *node,
                   const char *name,
                   Loader* bin,
                   std::vector<T> &arr)
    {
      std::string sName;
      offset_t offset = -1;
      int size = 0;
      if (node->QueryStringAttribute(name, &sName) != TIXML_SUCCESS)
      { throw std::runtime_error("Cannot get offset and size for array"); }

      size_t delim = sName.find(':');
      size_t len = sName.length();
      offset_t t[2];
      if (delim != std::string::npos)
        Converter::Parse(sName.substr(0, delim), t[0]);
      else
        throw std::runtime_error("Internal error at DeSerializer::DeSerializeArray(): cannot find delim");
      Converter::Parse(sName.substr(delim+1, len - delim), t[1]);

      if (t[0] >= 0 && t[1] > 0)
      {
        arr.resize((size_t)(t[1] / sizeof(T)));
        bin->LoadArray((size_t)t[0], (size_t)t[1], &arr[0]);
      }
      else
      { arr.resize(0); }
    }

  public:
    // Deserializes program version that was used to create file.
    // Returns "ProgramVersion" + "FileFormatVersion" bundle.
    // Never returns NULL, can throw std::runtime_error().
    static std::pair<Version, int> DeserializeVersion(uint64_t version);

    // Deserializes cell structure only. Returned cell data object will be unvalid untill cell state deserialization.
    // Returns "Cell" + "InitialRandSeed" bundle.
    // Never returns NULL, can throw std::runtime_error().
    static std::pair<Cell *, uint32_t> DeserializeCellConfiguration(TiXmlElement *configuration, void* data, size_t sizeInBytes);

    // Deserializes cell state (updates provided cell).
    // The "cell" object must be created via "DeserializeCellConfiguration()" method.
    // Returns "LayerTime" + "EndRandSeed" bundle.
    // Never returns NULL, can throw std::runtime_error().
    static std::pair<double, uint32_t> DeserializeTimeLayer(TiXmlElement *timeLayer, Cell *cell, void* data, size_t sizeInBytes);

    // Deserializes only time of the layer.
    // Can throw std::runtime_error().
    static double DeserializeTime(TiXmlElement *timeLayer);

    // Deserializes parameters and returns configured object.
    // Never returns NULL, can throw std::runtime_error().
    static SimParams *DeserializeSimParams(TiXmlElement *params, void* data, size_t sizeInBytes);
};

