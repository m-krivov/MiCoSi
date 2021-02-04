#pragma once

#include "Simulator.h"
#include "SimulatorConfig.h"

#include "Mitosis.Objects/Interfaces.h"


class SimulatorFactory
{
  public:
    SimulatorFactory() = delete;
    SimulatorFactory(const SimulatorFactory &) = delete;
    SimulatorFactory &operator =(const SimulatorFactory &) = delete;

    static std::unique_ptr<Simulator>
      Create(const std::vector<Random::State> &states,
             ICellInitializer *cellInitializer = nullptr,
             IPoleUpdater *poleUpdater = nullptr,
             SimulatorConfig config = SimulatorConfig::Default());

    static std::unique_ptr<Simulator>
      Create(const std::vector<std::pair<const Cell *, Random::State> > &cells,
             double startTime,
             IPoleUpdater *poleUpdater = nullptr,
             SimulatorConfig config = SimulatorConfig::Default());

  private:
    static std::unique_ptr<Simulator>
      CreateInternal(Simulator::CellEnsemble &cells,
                     double startTime, IPoleUpdater &poles,
                     const SimulatorConfig &config);
};
