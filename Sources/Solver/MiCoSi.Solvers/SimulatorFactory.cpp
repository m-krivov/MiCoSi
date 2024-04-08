#include "SimulatorFactory.h"

#include "MiCoSi.Objects/All.h"

#include "RandomCellInitializer.h"
#include "StaticPoleUpdater.h"
#include "CpuSimulator/CpuSimulator.h"

#ifndef NO_CUDA
#include "CudaSimulator/CudaSimulator.h"
#include "CudaSimulator2/CudaSimulator2.h"
#endif


//------------------------
//--- SimulatorFactory ---
//------------------------

std::unique_ptr<Simulator> SimulatorFactory::CreateInternal(Simulator::CellEnsemble &cells,
                                                            double startTime, IPoleUpdater &poles,
                                                            const SimulatorConfig &config)
{
  std::unique_ptr<Simulator> res;

  if (config.Type() == SimulatorConfig::CPU)
  {
    int cores = 0;
    if (!config.HasDeviceNumber(cores))
    { cores = 0; }
    res.reset(new CpuSimulator(poles, (size_t)cores));
  }
  else if (config.Type() == SimulatorConfig::CUDA)
  {
#ifndef NO_CUDA
    res = new CudaSimulator(poles);
    //res = new CudaSimulator2(poles);
#else
    throw std::runtime_error("you must recompile project with CUDA support in order to use CUDA solver");
#endif
  }
  else
  {
    throw std::runtime_error("internal error, unknown enum value of 'config.Type()'");
  }

  res->Init(cells, startTime);    
  return res;
}

std::unique_ptr<Simulator>SimulatorFactory::Create(const std::vector<Random::State> &states,
                                                   ICellInitializer *cellInitializer,
                                                   IPoleUpdater *poleUpdater,
                                                   SimulatorConfig config)
{
  std::unique_ptr<ICellInitializer> cellInitializer_;
  if (cellInitializer == nullptr)
  { cellInitializer_.reset(cellInitializer = new RandomCellInitialzier()); }

  std::unique_ptr<IPoleUpdater> poleUpdater_;
  if (poleUpdater == nullptr)
  { poleUpdater_.reset(poleUpdater = new StaticPoleUpdater()); }

  Simulator::CellEnsemble cells(states.size());
  for (size_t i = 0; i < cells.size(); i++)
  {
    cells[i] = std::make_unique<CellWithRng>(cellInitializer, poleUpdater, states[i]);
  }

  return CreateInternal(cells, 0.0, *poleUpdater, config);
}

std::unique_ptr<Simulator>
  SimulatorFactory::Create(const std::vector<std::pair<const Cell *, Random::State> > &cells,
                           double startTime,
                           IPoleUpdater *poleUpdater,
                           SimulatorConfig config)
{
  std::unique_ptr<IPoleUpdater> poleUpdater_;
  if (poleUpdater == nullptr)
  { poleUpdater_.reset(poleUpdater = new StaticPoleUpdater()); }

  std::vector<std::unique_ptr<CellWithRng> > cellsWithRng(cells.size());
  for (size_t i = 0; i < cells.size(); i++)
  {
    cellsWithRng[i] = std::make_unique<CellWithRng>(*cells[i].first, cells[i].second);
  }

  return CreateInternal(cellsWithRng, startTime, *poleUpdater, config);
}
