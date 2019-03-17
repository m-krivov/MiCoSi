
#pragma once

#include "tinyxml.h"
#include "Mitosis.Core/Versions.h"
#include "Mitosis.Core/SimParams.h"
#include "Mitosis.Objects/Cell.h"

#include "MemoryStream.h"

// Static class that performs serialization into XML-nodes.
class Serializer
{
  public:
    // Serializes simulator version that was used in order to create file.
    // Never returns NULL, can throw std::runtime_error().

    static uint64_t SerializeVersion(const Version *programVersion, int fileFormatVersion);

    // Serializes unchangeabale information about cell.
    // Never returns NULL, can throw std::runtime_error().
    static TiXmlElement *SerializeCellConfiguration(Cell *cell, uint32_t initialRandSeed, MemoryStream *stream);

    // Serializes time layer (only changeable values of the Cell parameters).
    // Never returns NULL, can throw std::runtime_error().
    static TiXmlElement *SerializeTimeLayer(Cell *cell, double time, uint32_t randSeed, MemoryStream *stream);

    // Serializes simulation parameters.
    // Never returns NULL, can throw std::runtime_error().
    static TiXmlElement *SerializeSimParams(SimParams *params, MemoryStream* stream);
};
