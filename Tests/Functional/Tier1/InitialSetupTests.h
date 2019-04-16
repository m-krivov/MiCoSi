#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline String ^InitialSetupChecker(TimeStream ^ts, Object ^obj)
{
  auto collections = safe_cast<Tuple<List<String ^> ^, List<String ^> ^> ^>(obj);
  auto MTs  = collections->Item1;
  auto chrs = collections->Item2;
  double normalizer = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true) / 3;

  if (ts->LayerCount != 1 + 1) { return "Wrong count of layers"; }
  ts->MoveTo(0);
  auto cell = ts->Current->Cell;
  if (Enumerable::Count(cell->MTs) != MTs->Count) { return "Wrong count of MTs"; }
  if (Enumerable::Count(cell->Chromosomes) != chrs->Count) { return "Wrong count of chromosomes"; }

  for (int i = 0; i < Enumerable::Count(cell->Chromosomes); i++)
  {
    auto chr = Enumerable::ElementAt(cell->Chromosomes, i);
    auto str = String::Format("x:{0} y:{1} z:{2}",
                              (int)(chr->Position->X * 1000 / normalizer),
                              (int)(chr->Position->Y * 1000 / normalizer),
                              (int)(chr->Position->Z * 1000 / normalizer));
    if (str != chrs[i]) { return "Initial state of chromosome was ignored"; }
  }

  for (int i = 0; i < Enumerable::Count(cell->MTs); i++)
  {
    auto mt = Enumerable::ElementAt(cell->MTs, i);
    auto str = String::Format("x:{0} y:{1} z:{2} l:{3} p:{4} s:{5}",
                              (int)(mt->Direction->X * 1000),
                              (int)(mt->Direction->Y * 1000),
                              (int)(mt->Direction->Z * 1000),
                              (int)(mt->Length * 1000 / normalizer),
                              mt->Pole->Type.ToString(),
                              mt->State.ToString());
    if (str != MTs[i]) { return "Initial state of MT was ignored"; }
  }

  return nullptr;
}

static inline void InitialSetupConfiguration(int chrPairs, int mtsPerPole,
                                             [Out] LaunchParameters ^%parameters, [Out] Object ^%obj)
{
  parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();

  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_Cr_Total] = chrPairs;
  parameters->Config[SimParameter::Int::N_MT_Total] = mtsPerPole;
  parameters->Config[SimParameter::Double::Dt] = 1.0;
  parameters->Config[SimParameter::Double::T_End] = 1.0;
  parameters->Config[SimParameter::Double::F_Cat] = 0.0;

  auto MTs  = gcnew List<String ^>();
  auto chrs = gcnew List<String ^>();
  obj = gcnew Tuple<List<String ^> ^, List<String ^> ^>(MTs, chrs);

  auto rnd = gcnew Random(Environment::TickCount);
  double normalizer = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true) / 3;
  for (int i = 0; i < mtsPerPole * 2; i++)
  {
    auto poleType = i < mtsPerPole
                  ? PoleType::Left
                  : PoleType::Right;
    double direction_x = rnd->NextDouble() + 1e-1;
    direction_x = poleType == PoleType::Left ? direction_x : -direction_x;
    double direction_y = rnd->NextDouble();
    double direction_z = rnd->NextDouble();
    double n = Math::Sqrt(direction_x * direction_x +
                          direction_y * direction_y +
                          direction_z * direction_z);
    direction_x /= n;
    direction_y /= n;
    direction_z /= n;
    double length = rnd->NextDouble() * normalizer;
    auto state = MTState::Polymerization;

    parameters->InitialStates->AddMT(poleType,
                                     direction_x, direction_y, direction_z,
                                     length,
                                     state);
    MTs->Add(String::Format("x:{0} y:{1} z:{2} l:{3} p:{4} s:{5}",
                            (int)(direction_x * 1000),
                            (int)(direction_y * 1000),
                            (int)(direction_z * 1000),
                            (int)(length * 1000 / normalizer),
                            poleType.ToString(), state.ToString()));
  }

  for (int i = 0; i < chrPairs; i++)
  {
    double position_x = (rnd->NextDouble() / 2 - 1.0) * normalizer;
    double position_y = (rnd->NextDouble() / 2 - 1.0) * normalizer;
    double position_z = (rnd->NextDouble() / 2 - 1.0) * normalizer;

    parameters->InitialStates->AddChromosomePair(position_x, position_y, position_z, 0.0, 0.0, 0.0);
    chrs->Add(String::Format("x:{0} y:{1} z:{2}", (int)(1000 * position_x / normalizer),
                             (int)(1000 * position_y / normalizer),
                             (int)(1000 * position_z / normalizer)));
    chrs->Add(String::Format("x:{0} y:{1} z:{2}", (int)(1000 * position_x / normalizer),
                             (int)(1000 * position_y / normalizer),
                             (int)(1000 * position_z / normalizer)));
  }
}

TEST(InitialSetup, 2cr_2mt)
{
  LaunchParameters ^parameters; Object ^obj;
  InitialSetupConfiguration(1, 1, parameters, obj);
  UNIFIED_TEST(parameters, InitialSetupChecker, obj);
}

TEST(InitialSetup, 48cr_1024t)
{
  LaunchParameters ^parameters; Object ^obj;
  InitialSetupConfiguration(24, 512, parameters, obj);
  UNIFIED_TEST(parameters, InitialSetupChecker, obj);
}

#define INITIAL_SETUP_FAILED_LAUNCH(parameters)                                                   \
{                                                                                                 \
  bool exceptionThrown = false;                                                                   \
  TimeStream ^ts = nullptr;                                                                       \
  try                                                                                             \
  {                                                                                               \
    Helper::PrepareTestDirectory();                                                               \
    ts = Helper::LaunchAndOpen(parameters);                                                       \
  }                                                                                               \
  catch (Exception ^) { exceptionThrown = true; }                                                 \
  finally                                                                                         \
  {                                                                                               \
    if (ts != nullptr) { delete ts; }                                                             \
    Helper::ClearUpTestDirectory();                                                               \
  }                                                                                               \
  if (!exceptionThrown)                                                                           \
  { FAIL() << StringToString("Simulation with wrong setup was successfully completed"); }         \
}

TEST(InitialSetup, WithErrors)
{
  double r_cell = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);

  // Chromosome is out of cell
  {
    auto parameters = gcnew LaunchParameters();
    parameters->Config = gcnew SimParams();
    parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
    parameters->Config[SimParameter::Int::N_MT_Total] = 0;
    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddChromosomePair(0.0, r_cell * 2, 0.0, 0.0, 0.0, 0.0);
    INITIAL_SETUP_FAILED_LAUNCH(parameters);
  }

  // MT's end point is out of cell
  {
    auto parameters = gcnew LaunchParameters();
    parameters->Config = gcnew SimParams();
    parameters->Config[SimParameter::Int::N_Cr_Total] = 0;
    parameters->Config[SimParameter::Int::N_MT_Total] = 1;
    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, r_cell * 2, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.0, 0.0, r_cell * 3, MTState::Polymerization);
    INITIAL_SETUP_FAILED_LAUNCH(parameters);
  }

  // Left-right numbers are not equal
  {
    auto parameters = gcnew LaunchParameters();
    parameters->Config = gcnew SimParams();
    parameters->Config[SimParameter::Int::N_Cr_Total] = 0;
    parameters->Config[SimParameter::Int::N_MT_Total] = 2;
    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Left, -1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    INITIAL_SETUP_FAILED_LAUNCH(parameters);
  }
}
