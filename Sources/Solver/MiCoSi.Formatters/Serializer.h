#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/Versions.h"
#include "MiCoSi.Core/SimParams.h"
#include "MiCoSi.Objects/Cell.h"

#include <tinyxml.h>
#include "MemoryStream.h"

// Static class that performs serialization into XML-nodes
class Serializer
{
  public:
    Serializer() = delete;
    Serializer(const Serializer &) = delete;
    Serializer &operator =(const Serializer &) = delete;

    // Serializes simulator version that was used in order to create file
    static uint64_t SerializeVersion(const Version &programVersion,
                                     const std::string &flags,
                                     int fileFormatVersion);

    // Serializes the unchangeabale information about cell
    static TiXmlElement *SerializeCellConfiguration(const Cell &cell,
                                                    const Random::State &rngState,
                                                    int64_t rngSeed,
                                                    MemoryStream &stream);

    // Serializes time layer (only changeable values of the Cell's parameters)
    static TiXmlElement *SerializeTimeLayer(const Cell &cell, double time,
                                            const Random::State &rng, MemoryStream &stream);

    // Serializes simulation parameters
    static TiXmlElement *SerializeSimParams(const SimParams &params, MemoryStream &stream);
};
