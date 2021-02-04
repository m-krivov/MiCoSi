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

namespace
{

std::string ReadAllText(const std::string &filename)
{
  std::ifstream f(filename);

  if (f)
  {
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
  }
  else
  {
    std::ostringstream ss;
    ss << std::string("failed to read content of the file '") + filename + "'";
    throw std::runtime_error(ss.str());
  }
}

std::unique_ptr<Simulation> StartSimulation(const char *cellFile,
                                            const char *configFile,
                                            const char *initialConditions,
                                            const char *poleCoords,
                                            const std::vector<Random::State> &rngStates,
                                            int64_t userSeed,
                                            SimulatorConfig config)
{
  std::unique_ptr<Simulator> sim;
  std::vector<std::unique_ptr<TimeStream> > ts;

  // Load simulation parameters
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

  // Create simulator and time streams
  sim = SimulatorFactory::Create(rngStates, cellInitializer.get(), poleUpdater.get(), config);
  decltype(auto) cells = sim->Cells();
  auto filenames = MitosisArgs::MultiplyCells(cellFile, rngStates.size());
  for (size_t i = 0; i < rngStates.size(); i++)
  {
    auto cur = TimeStream::Create(filenames[i].c_str(),
                                  cells[i]->CellObject(),
                                  rngStates[i], userSeed);
    cur->Append(*GlobalSimParams::GetRef());
    ts.emplace_back(std::move(cur));
  }

  // Store the first time layer
  for (size_t i = 0; i < cells.size(); i++)
  { ts[i]->Append(cells[i]->CellObject(), 0.0, cells[i]->Rng()); }

  return std::unique_ptr<Simulation>(new Simulation(sim, ts));
}

} // unnamed namespace

std::pair<size_t, double> WorkingDirUtility::Fix(const char *cellFile)
{
  std::unique_ptr<TimeStream> ts(TimeStream::Repair(cellFile));
  size_t layers = ts->LayerCount();
  double lastTime = ts->GetLayerTime(layers - 1);
  return std::make_pair(layers, lastTime);
}

std::unique_ptr<Simulation> WorkingDirUtility::Start(const char *cellFile,
                                                     const char *configFile,
                                                     const char *initialConditions,
                                                     const char *poleCoords,
                                                     size_t cellCount,
                                                     int64_t userSeed,
                                                     SimulatorConfig config)
{
  auto initRng = [userSeed](Random::State &state) -> void {
    if (userSeed < 0) { Random::Initialize(state); }
    else { Random::Initialize(state, (uint32_t)userSeed); }
  };

  std::vector<Random::State> states(cellCount);
  if (states.size() == 1)
  { initRng(states[0]); }
  else
  {
    Random::State tmp;
    initRng(tmp);
    Random::Multiply(tmp, states);
  }

  return StartSimulation(cellFile, configFile,
                         initialConditions, poleCoords,
                         states, userSeed, config);
}

std::unique_ptr<Simulation> WorkingDirUtility::Restart(const char *cellFile,
                                                       const char *configFile,
                                                       const char *initialConditions,
                                                       const char *poleCoords,
                                                       size_t cellCount,
                                                       SimulatorConfig config)
{
  int64_t userSeed = -1;
  std::vector<Random::State> states(cellCount);
  if (cellCount == 1)
  {
    std::unique_ptr<TimeStream> ts(TimeStream::Open(cellFile));
    states[0] = ts->InitialRNG();
    userSeed  = ts->UserSeed();
  }
  else
  {
    auto filenames = MitosisArgs::MultiplyCells(cellFile, cellCount);
    for (size_t i = 0; i < cellCount; i++)
    {
      std::unique_ptr<TimeStream> ts(TimeStream::Open(filenames[i].c_str()));
      states[i] = ts->InitialRNG();
    }
  }

  return StartSimulation(cellFile, configFile, initialConditions, poleCoords, states, userSeed, config);
}

std::unique_ptr<Simulation> WorkingDirUtility::Continue(const char *cellFile,
                                                        const char *configFile,
                                                        const char *initialConditions,
                                                        const char *poleCoords,
                                                        size_t cellCount,
                                                        SimulatorConfig config)
{
  if (initialConditions != nullptr)
  {
    throw std::runtime_error(
      "incompatible options, cannot continue an existent simulation with the declared initial conditions"
    );
  }

  std::vector<std::pair<const Cell *, Random::State> > cells;
  std::vector<std::unique_ptr<TimeStream> > ts;
  double time = -1.0;

  // Create pole updater
  std::unique_ptr<IPoleUpdater> poleUpdater;
  if (poleCoords != nullptr)
  { poleUpdater.reset(new XmlPoleUpdater(poleCoords)); }

  // Load the previous cell states
  auto filenames = MitosisArgs::MultiplyCells(cellFile, cellCount);
  for (size_t i = 0; i < cellCount; i++)
  {
    // Open existent time stream
    auto cur = TimeStream::Open(filenames[i].c_str());
    if (cur->LayerCount() == 0)
    { throw std::runtime_error("cannot continue empty simulation, you should use 'new' mode"); }

    cur->MoveTo(cur->LayerCount() - 1);

    // Set (or check) the simulation parameters
    std::unique_ptr<SimParams> params(new SimParams());
    params->SetAccess(SimParams::Access::Initialize);
    params->ImportValues(cur->Current().GetSimParams().ExportValues());
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
      {
        throw std::runtime_error(
            "cannot process cells with different parameters simultanoiusly (internal error)"
        );
      }

      for (size_t i = 0; i < myParams.size(); i++)
      {
        if (myParams[i].first != glParams[i].first || myParams[i].second != glParams[i].second)
        {
          throw std::runtime_error(
              "cannot process cell with different parameters simultaniously (wrong values)"
          ); }
      }

      if (time != cur->Current().GetTime())
      { throw std::runtime_error("cannot process cells with different times simultanoiusly"); }
    }

    auto tl = cur->Current();
    cells.push_back(std::make_pair(&tl.GetCell(), tl.GetRng()));
    ts.emplace_back(std::move(cur));
  }

  // Finally, create the simulator
  auto sim = SimulatorFactory::Create(cells, time, poleUpdater.get(), config);

  return std::make_unique<Simulation>(sim, ts);
}
