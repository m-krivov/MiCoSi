
#pragma once

#include "Simulation.h"

// Performs basic actions with working directory.
// All methods can throw std::exception();
class WorkingDirUtility
{
  private:
    static std::string ReadAllText(const std::string &filename);

    static Simulation *Start(const char *cellFile,
                 const char *configFile,
                 const char *initialConditions,
                 const char *poleCoords,
                 const std::vector<uint32_t> &seeds,
                 SimulatorConfig config);

  public:
    // Tries to repair broken file. Returns number + time of the last layer.
    static std::pair<size_t, double> Fix(const char *cellFile);

    // Creates new time streams, associated with provided cell files.
    // Returned objects must be destructed by caller.
    static Simulation *Start(const char *cellFile,
                 const char *configFile,
                 const char *initialConditions,
                 const char *poleCoords,
                 size_t cellCount,
                 uint32_t userSeed,
                 SimulatorConfig config);

    // Creates new time streams, associated with provided cell files, but with the same RNG.
    // Returned objects must be destructed by caller.
    static Simulation *Restart(const char *cellFile,
                   const char *configFile,
                   const char *initialConditions,
                   const char *poleCoords,
                   size_t cellCount,
                   SimulatorConfig config);

    // Opens existant time streams, that are stored in cell files.
    // Returned objects must be destructed by caller.
    static Simulation *Continue(const char *cellFile,
                  const char *configFile,
                  const char *initialConditions,
                  const char *poleCoords,
                  size_t cellCount,
                  SimulatorConfig config);
};
