
#include "WorkingDirUtility.h"

#include "Mitosis.Core/Random.h"
#include "Mitosis.Solvers/All.h"
#include "Mitosis.Formatters/SimParamsFormatter.h"
#include "ConsoleApp.Args/MitosisArgs.h"

#include "XmlCellInitializer.h"
#include "XmlPoleUpdater.h"

//-------------------------
//--- WorkingDirUtility ---
//-------------------------

std::string WorkingDirUtility::ReadAllText(const std::string &filename)
{
  std::ifstream f(filename);

  if (f)
  {
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
  }
  else
    throw std::runtime_error(std::string("Failed to read content of the file \"") + filename + "\"");
}

std::pair<size_t, double> WorkingDirUtility::Fix(const char *cellFile)
{
  std::unique_ptr<TimeStream> ts(TimeStream::Repair(cellFile));
  size_t layers = ts->LayerCount();
  double lastTime = ts->GetLayerTime(layers - 1);
  return std::make_pair(layers, lastTime);
}

Simulation *WorkingDirUtility::Start(const char *cellFile,
                                     const char *configFile,
                                     const char *initialConditions,
                                     const char *poleCoords,
                                     const std::vector<uint32_t> &seeds,
                                     SimulatorConfig config)
{
  Simulator *sim = NULL;
  std::vector<TimeStream *> ts;

  try
  {
    // Loading simulation parameters
    GlobalSimParams::GetRef()->SetAccess(SimParams::Access::Initialize);
    SimParamsFormatter::ImportAsProps(GlobalSimParams::GetRef(), ReadAllText(configFile));
    GlobalSimParams::GetRef()->SetAccess(SimParams::Access::Update);

    // And initializers
    std::unique_ptr<ICellInitializer> cellInitializer;
    if (initialConditions != nullptr)
    { cellInitializer.reset(new XmlCellInitializer(initialConditions)); }
    std::unique_ptr<IPoleUpdater> poleUpdater;
    if (poleCoords != nullptr)
    { poleUpdater.reset(new XmlPoleUpdater(poleCoords)); }

    // Creating simulator.
    sim = SimulatorFactory::Create(seeds, cellInitializer.get(), poleUpdater.get(), config);

    // Creating time streams.
    auto filenames = MitosisArgs::MultiplyCells(cellFile, seeds.size());
    for (size_t i = 0; i < seeds.size(); i++)
    {
      auto cur = TimeStream::Create(filenames[i].c_str(), sim->Cells()[i].first, seeds[i]);
      ts.push_back(cur);
      cur->Append(GlobalSimParams::GetRef());
    }

    // Storing initial states.
    for (size_t i = 0; i < seeds.size(); i++)
      ts[i]->Append(sim->Cells()[i].first, 0.0, *sim->Cells()[i].second);

    return new Simulation(sim, ts);
  }
  catch (std::exception &)
  {
    if (sim != NULL)
      delete sim;

    for (size_t i = 0; i < ts.size(); i++)
      if (ts[i] != NULL) delete ts[i];

    throw;
  }
}

Simulation *WorkingDirUtility::Start(const char *cellFile,
                   const char *configFile,
                   const char *initialConditions,
                   const char *poleCoords,
                   size_t cellCount,
                   uint32_t userSeed,
                   SimulatorConfig config)
{
  uint32_t seed = userSeed;
  
  std::vector<uint32_t> seeds(cellCount);
  if (seeds.size() == 1)
    seeds[0] = seed;
  else
    for (size_t i = 0; i < seeds.size(); i++)
      seeds[i] = Random::Func::TypeB(seed);

  return Start(cellFile, configFile, initialConditions, poleCoords, seeds, config);
}

Simulation *WorkingDirUtility::Restart(const char *cellFile,
                     const char *configFile,
                     const char *initialConditions,
                     const char *poleCoords,
                     size_t cellCount,
                     SimulatorConfig config)
{
  std::vector<uint32_t> seeds(cellCount);

  auto filenames = MitosisArgs::MultiplyCells(cellFile, cellCount);
  for (size_t i = 0; i < cellCount; i++)
  {
    std::unique_ptr<TimeStream> ts(TimeStream::Open(filenames[i].c_str()));
    seeds[i] = ts->InitialRandSeed();
  }

  return Start(cellFile, configFile, initialConditions, poleCoords, seeds, config);
}

Simulation *WorkingDirUtility::Continue(const char *cellFile,
                    const char *configFile,
                    const char *initialConditions,
                    const char *poleCoords,
                    size_t cellCount,
                    SimulatorConfig config)
{
  if (initialConditions != NULL)
    throw std::runtime_error("Incompatable options - cannot continue existent simulation with declared initial conditions");

  Simulator *sim = NULL;
  std::vector<std::pair<Cell *, uint32_t> > cells;
  std::vector<TimeStream *> ts;
  double time = -1.0;

  try
  {
    // Loading pole updater
    std::unique_ptr<IPoleUpdater> poleUpdater;
    if (poleCoords != nullptr)
    {
      poleUpdater.reset(new XmlPoleUpdater(poleCoords));
    }

    // Loading the previous state
    auto filenames = MitosisArgs::MultiplyCells(cellFile, cellCount);
    for (size_t i = 0; i < cellCount; i++)
    {
      // Opening existent time stream.
      auto cur = TimeStream::Open(filenames[i].c_str());
      ts.push_back(cur);

      if (cur->LayerCount() == 0)
        throw std::runtime_error("Cannot continue empty simulation, use \"new\" mode");
      cur->MoveTo(cur->LayerCount() - 1);

      // Setting or checking simulation parameters.
      std::unique_ptr<SimParams> params(new SimParams());
      params->SetAccess(SimParams::Access::Initialize);
      params->ImportValues(cur->Current().GetSimParams()->ExportValues());
      params->SetAccess(SimParams::Access::Update);
      SimParamsFormatter::ImportAsProps(params.get(), ReadAllText(configFile));

      if (i == 0)  // the first cell sets global parameters
      {
        GlobalSimParams::GetRef()->SetAccess(SimParams::Access::Initialize);
        GlobalSimParams::GetRef()->ImportValues(params->ExportValues());
        GlobalSimParams::GetRef()->SetAccess(SimParams::Access::Update);
        time = cur->Current().GetTime();
      }
      else // others check correctness of their own parameters
      {
        auto myParams = params->ExportValues();
        auto glParams = params->ExportValues();
        if (myParams.size() != glParams.size())
          throw std::runtime_error("Cannot process cells with different parameters simultanoiusly (internal error)");

        for (size_t i = 0; i < myParams.size(); i++)
          if (myParams[i].first != glParams[i].first || myParams[i].second != glParams[i].second)
            throw std::runtime_error("Cannot process cell with different parameters simultaniously (wrong values)");

        if (time != cur->Current().GetTime())
          throw std::runtime_error("Cannot process cells with different times simultanoiusly");
      }

      // Setting RNG seed.
      cells.push_back(std::make_pair(cur->Current().GetCell(), cur->Current().GetRandSeed()));
    }

    // Creating simulator.
    sim = SimulatorFactory::Create(cells, time, poleUpdater.get(), config);

    return new Simulation(sim, ts);
  }
  catch (std::exception &)
  {
    if (sim != NULL)
      delete sim;

    for (size_t i = 0; i < ts.size(); i++)
      if (ts[i] != NULL) delete ts[i];

    throw;
  }
}
