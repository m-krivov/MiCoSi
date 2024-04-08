#pragma once
#include "MiCoSi.Core/Defs.h"

#include "MiCoSi.Core/Versions.h"
#include "MiCoSi.Core/SimParams.h"
#include "MiCoSi.Objects/Cell.h"

#include <tinyxml.h>
#include "MemoryStream.h"


// Static class that performs deserialization of the internal objects
class DeSerializer
{
   public:
    DeSerializer() = delete;
    DeSerializer(const DeSerializer &) = delete;
    DeSerializer &operator =(const DeSerializer &) = delete;

    // Deserializes program version that was used to create file
    // Returns "ProgramVersion" + "FileFormatVersion" pair
    static std::tuple<Version, std::string, int> DeserializeVersion(uint64_t version);

    // Deserialize flags with wich the program was compiled
    static std::string DeserializeCompilationFlags(const TiXmlElement *flags);

    // Deserializes state and seed that were used to initialize RNG
    // Negative seed values mean that no such seed exist
    static std::pair<Random::State, int64_t> DeserializeRng(const TiXmlElement *configuration,
                                                            const void *data, size_t sizeInBytes);

    // Deserializes cell structure only
    // The returned cell data object will be invalid until cell state deserialization
    static std::unique_ptr<Cell> DeserializeCellConfiguration(const TiXmlElement *configuration,
                                                              const void *data, size_t sizeInBytes);

    // Deserializes cell state (updates the provided cell)
    // The "cell" object must be created via "DeserializeCellConfiguration()" method
    // Returns "LayerTime" + "EndRandState" pair
    static std::pair<double, Random::State> DeserializeTimeLayer(const TiXmlElement *timeLayer, Cell &cell,
                                                                 const void *data, size_t sizeInBytes);

    // Deserializes only time of the layer
    static double DeserializeTime(const TiXmlElement *timeLayer);

    // Deserializes parameters and returns the configured object
    static std::unique_ptr<SimParams> DeserializeSimParams(const TiXmlElement *params,
                                                           const void *data, size_t sizeInBytes);
};
