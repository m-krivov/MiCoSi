#pragma once
#include "MiCoSi.Core/Defs.h"

#include "Simulation.h"

// Performs basic actions with working directory
// All methods can throw std::exception()
class WorkingDirUtility
{
  public:
    WorkingDirUtility() = delete;
    WorkingDirUtility(const WorkingDirUtility &) = delete;
    WorkingDirUtility &operator =(const WorkingDirUtility &) = delete;

    // Tries to repair the broken file
    // Returns number + time of the last layer
    static std::pair<size_t, double> Fix(const char *cellFile);

    // Creates new time streams, associated with provided cell files
    static std::unique_ptr<Simulation> Start(const char *cellFile,
                                             const char *configFile,
                                             const char *initialConditions,
                                             const char *poleCoords,
                                             size_t cellCount,
                                             int64_t userSeed,
                                             SimulatorConfig config);

    // Creates new time streams, associated with provided cell files, but with the same RNG
    static std::unique_ptr<Simulation> Restart(const char *cellFile,
                                               const char *configFile,
                                               const char *initialConditions,
                                               const char *poleCoords,
                                               size_t cellCount,
                                               SimulatorConfig config);

    // Opens existant time streams, that are stored in cell files
    static std::unique_ptr<Simulation> Continue(const char *cellFile,
                                                const char *configFile,
                                                const char *initialConditions,
                                                const char *poleCoords,
                                                size_t cellCount,
                                                SimulatorConfig config);
};
