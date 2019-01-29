
#pragma once

#include "Simulator.h"
#include "SimulatorConfig.h"

#include "Mitosis.Objects/Interfaces.h"

class SimulatorFactory
{
  private:
    static Simulator *CreateInternal(const std::vector<std::pair<Cell *, uint32_t> > &cells,
                                     double startTime, IPoleUpdater &poles,
                                     const SimulatorConfig &config);

  public:
    static Simulator *Create(const std::vector<uint32_t> &seeds,
                             ICellInitializer *cellInitializer = nullptr,
                             IPoleUpdater *poleUpdater = nullptr,
                             SimulatorConfig config = SimulatorConfig::Default());

    static Simulator *Create(const std::vector<std::pair<Cell *, uint32_t> > &cells,
                             double startTime,
                             IPoleUpdater *poleUpdater = nullptr,
                             SimulatorConfig config = SimulatorConfig::Default());
};
