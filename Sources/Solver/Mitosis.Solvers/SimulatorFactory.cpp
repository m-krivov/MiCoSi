
#include "SimulatorFactory.h"

#include "RandomCellInitializer.h"
#include "StaticPoleUpdater.h"
#include "CpuSimulator/CpuSimulator.h"

#include "Mitosis.Objects/All.h"

#ifndef NO_CUDA
#include "CudaSimulator/CudaSimulator.h"
#include "CudaSimulator2/CudaSimulator2.h"
#endif


//------------------------
//--- SimulatorFactory ---
//------------------------

Simulator *SimulatorFactory::CreateInternal(const std::vector<std::pair<Cell *, uint32_t> > &cells,
                                            double startTime, IPoleUpdater &poles,
                                            const SimulatorConfig &config)
{
  Simulator *res = NULL;

  try
  {
    if (config.Type() == SimulatorConfig::CPU)
    {
      int cores = 0;
      if (!config.HasDeviceNumber(cores))
      { cores = 0; }
      res = new CpuSimulator(poles, (size_t)cores);
    }
    else if (config.Type() == SimulatorConfig::CUDA)
    {
#ifndef NO_CUDA
      res = new CudaSimulator(poles);
      //res = new CudaSimulator2(poles);
#else
      throw std::runtime_error("You must recompile project with CUDA support in order to use CUDA solver");
#endif
    }
    else
    {
      throw std::runtime_error("internal error, unknown enum value of 'config.Type()'");
    }

    std::vector<std::pair<Cell *, uint32_t *> > _cells;
    for(size_t i = 0; i < cells.size(); i++)
    { _cells.push_back(std::make_pair(cells[i].first, new uint32_t(cells[i].second))); }
    res->Init(_cells, startTime);
    
    return res;
  }
  catch (std::exception &)
  {
    if (res != NULL) delete res;
    throw;
  }
}

Simulator *SimulatorFactory::Create(const std::vector<uint32_t> &seeds,
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

  std::vector<std::pair<Cell *, uint32_t> > cells(seeds.size());
  try
  {
    for (size_t i = 0; i < cells.size(); i++)
    {
      uint32_t cseed = seeds[i];
      Cell *cell = new Cell(cellInitializer, poleUpdater, cseed);
      cells[i] = std::make_pair(cell, cseed);
    }
  }
  catch (std::exception &)
  {
    for (size_t i = 0; i < cells.size(); i++)
      if (cells[i].first != NULL) delete cells[i].first;
    throw;
  }

  return CreateInternal(cells, 0.0, *poleUpdater, config);
}

Simulator *SimulatorFactory::Create(const std::vector<std::pair<Cell *, uint32_t> > &cells,
                                    double startTime,
                                    IPoleUpdater *poleUpdater,
                                    SimulatorConfig config)
{
  std::unique_ptr<IPoleUpdater> poleUpdater_;
  if (poleUpdater == nullptr)
  { poleUpdater_.reset(poleUpdater = new StaticPoleUpdater()); }

  std::vector<std::pair<Cell *, uint32_t> > clonnedCells(cells.size());
  try
  {
    for (size_t i = 0; i < cells.size(); i++)
      clonnedCells[i] = std::make_pair(cells[i].first->CloneTemplated<Cell>(), cells[i].second);
  }
  catch (std::exception &)
  {
    for (size_t i = 0; i < clonnedCells.size(); i++)
      if (clonnedCells[i].first != NULL) delete clonnedCells[i].first;
    throw;
  }

  return CreateInternal(clonnedCells, startTime, *poleUpdater, config);
}
