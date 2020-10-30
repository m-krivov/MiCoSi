
#pragma once

#include "Simulator.h"
#include "SimulatorConfig.h"

#include "Mitosis.Objects/Interfaces.h"

class SimulatorFactory
{
  public:
    static Simulator *Create(const std::vector<uint32_t> &seeds,
                             ICellInitializer *cellInitializer = nullptr,
                             IPoleUpdater *poleUpdater = nullptr,
                             SimulatorConfig config = SimulatorConfig::Default());

    static Simulator *Create(const std::vector<std::pair<Cell *, uint32_t> > &cells,
                             double startTime,
                             IPoleUpdater *poleUpdater = nullptr,
                             SimulatorConfig config = SimulatorConfig::Default());

  private:
    static Simulator *CreateInternal(const std::vector<std::pair<Cell *, uint32_t> > &cells,
                                     double startTime, IPoleUpdater &poles,
                                     const SimulatorConfig &config);
};
