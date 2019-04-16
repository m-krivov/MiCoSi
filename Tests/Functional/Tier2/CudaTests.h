#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline String ^CudaCheckNothing(TimeStream ^ts, Object ^obj) { return nullptr; }

static inline String ^CudaCheckBrokenSpring(TimeStream ^ts, Object ^obj)
{
  ts->MoveTo(ts->LayerCount - 1);
  if (!ts->Current->Cell->AreSpringsBroken)
  { return "This test is obsolete - springs are not broken"; }
  return nullptr;
}

static inline void CudaLaunch(LaunchParameters ^parameters)
{
  Launcher ^launcher = nullptr;
  try
  {
    launcher = gcnew Launcher(Helper::TestDirectory, Helper::SimulatorFile, parameters);
    {
      auto res = launcher->StartAndWait();
      if (res->ExitedWithError)
      { throw gcnew ApplicationException("Failed to launch the simulation process"); }
    }
  }
  finally
  { if (launcher != nullptr) { delete launcher; } }
}

static inline String ^CudaTest(LaunchParameters ^parameters, String ^ (*checker)(TimeStream ^, Object ^))
{
  Helper::PrepareTestDirectory();
  cli::array<IntPtr> ^gold = gcnew cli::array<IntPtr>(parameters->Args->CellCount),
                     ^cuda = gcnew cli::array<IntPtr>(parameters->Args->CellCount);
  for (int i = 0; i < parameters->Args->CellCount; i++)
  {
    gold[i] = (IntPtr)nullptr;
    cuda[i] = (IntPtr)nullptr;
  }
  if (parameters->Args->RngSeed < 0)
  { parameters->Args->RngSeed = (UInt32)Math::Abs(Environment::TickCount); }

  try
  {
    // Start the gold version
    parameters->Args->Solver = gcnew SimulatorConfig(SimulatorType::CPU);
    parameters->Args->Mode = LaunchMode::New;
    CudaLaunch(parameters);
    auto res = Directory::GetFiles(Helper::TestDirectory,
                                   parameters->Args->CellFile->Replace(".cell", "*.cell"),
                                   SearchOption::TopDirectoryOnly);
    if (res->Length != parameters->Args->CellCount)
    { return "Wrong count of cell files (gold solver)"; }

    for (int i = 0; i < res->Length; i++)
    {
      TimeStream ^ts = nullptr;
      try
      {
        ts = TimeStream::Open(res[i]);
        auto ret = checker(ts, nullptr);
        if (!String::IsNullOrEmpty(ret)) { return ret; }
        ts->MoveTo(ts->LayerCount - 1);
        gold[i] = Helper::CopyData(ts->Current->Cell);
      }
      finally { if (ts != nullptr) { delete ts; } }
    }
    Helper::ClearUpTestDirectory();
    Helper::PrepareTestDirectory();

    // Start the CUDA-accelerated version
    parameters->Args->Solver = gcnew SimulatorConfig(SimulatorType::CUDA);
    parameters->Args->Mode = LaunchMode::New;
    CudaLaunch(parameters);
    res = Directory::GetFiles(Helper::TestDirectory,
                              parameters->Args->CellFile->Replace(".cell", "*.cell"),
                              SearchOption::TopDirectoryOnly);
    if (res->Length != parameters->Args->CellCount)
    { return "Wrong count of cell files (cuda solver)"; }
    for (int i = 0; i < res->Length; i++)
    {
      TimeStream ^ts = nullptr;
      try
      {
        ts = TimeStream::Open(res[i]);
        auto ret = checker(ts, nullptr);
        if (!String::IsNullOrEmpty(ret)) { return ret; }
        ts->MoveTo(ts->LayerCount - 1);
        cuda[i] = Helper::CopyData(ts->Current->Cell);
      }
      finally { if (ts != nullptr) { delete ts; } }
    }

    //Comparing
    for (int i = 0; i < parameters->Args->CellCount; i++)
    {
      if (!Helper::CompareData(gold[i], cuda[i]))
      { return "CUDA solver provided different results"; }
    }
  }
  catch (Exception ^ex) { return ex->Message; }
  finally
  {
    Helper::ClearUpTestDirectory();
    for (int i = 0; i < parameters->Args->CellCount; i++)
    {
      if (gold[i] != (IntPtr)nullptr) Helper::ReleaseData(gold[i]);
      if (cuda[i] != (IntPtr)nullptr) Helper::ReleaseData(cuda[i]);
    }
  }

  return nullptr;
}

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, 5MT_1Chr)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
  parameters->Config[SimParameter::Int::N_MT_Total] = 5;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, 750MT_23Chr)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 23;
  parameters->Config[SimParameter::Int::N_MT_Total] = 750;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;
  parameters->Config[SimParameter::Double::T_End] = 50.0;

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, NoMTs)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 16;
  parameters->Config[SimParameter::Int::N_MT_Total] = 0;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, NoChrs)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 0;
  parameters->Config[SimParameter::Int::N_MT_Total] = 1000;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, NoMTs_NoChrs)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 0;
  parameters->Config[SimParameter::Int::N_MT_Total] = 0;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, Diffusion)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 4;
  parameters->Config[SimParameter::Int::N_MT_Total] = 500;
  parameters->Config[SimParameter::Double::D_Rot] = 0.01;
  parameters->Config[SimParameter::Double::D_Trans] = 0.01;
  parameters->Config[SimParameter::Double::T_End] = 400;

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, InitialSetup_MovingPoles)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 3;
  parameters->Config[SimParameter::Int::N_MT_Total] = 100;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;
  parameters->Config[SimParameter::Double::T_End] = 400;
  double r_cell = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);

  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, r_cell / 2, 0.0, 0.0, 0.0);
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, -r_cell / 2, 0.0, 0.0, 0.0);
  for (int i = 0; i < 100; i++)
  {
    double angle = 8 * Math::PI * (double)i / 99;
    double mult = 1.0 / Math::PI;
    parameters->InitialStates->AddMT(PoleType::Left, 1.0,
                                     angle * Math::Sin(angle) * mult,
                                     angle * Math::Cos(angle) * mult,
                                     0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0,
                                     angle * Math::Cos(angle) * mult,
                                     angle * Math::Sin(angle) * mult,
                                     0.0, MTState::Polymerization);
  }

  double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);
  parameters->PoleCoords = gcnew PoleCoordinates();
  parameters->PoleCoords->AddRecord(0.0, -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(50.0, -l_poles / 2, r_cell / 5, 0.0, l_poles / 2, -r_cell / 5, 0.0);
  parameters->PoleCoords->AddRecord(150.0, -l_poles / 2, -r_cell / 5, 0.0, l_poles / 2, r_cell / 5, 0.0);

  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, BrokenSpring)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
  parameters->Config[SimParameter::Int::N_MT_Total] = 1000;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;
  parameters->Config[SimParameter::Double::T_End] = 80;
  parameters->Config[SimParameter::Int::Spring_Brake_Type] = 1;
  parameters->Config[SimParameter::Int::Spring_Brake_MTs] = 15;
  parameters->Config[SimParameter::Double::Spring_Length] = 0.1;
  parameters->Config[SimParameter::Double::K_On] = 1.0 / SimParams::GetDefaultValue(SimParameter::Double::Dt, true);
  parameters->Config[SimParameter::Double::K_Off] = 0.0;
  double r_cell = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);

  parameters->Config[SimParameter::Double::Const_A]
    = SimParams::GetDefaultValue(SimParameter::Double::Const_A, false) / 5;
  parameters->Config[SimParameter::Double::F_Cat] = 0.0;

  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  for (int i = 0; i < 1000; i++)
  {
    double angle = 8 * Math::PI * (double)i / 999;
    double mult = 1.0 / Math::PI / 16;
    parameters->InitialStates->AddMT(PoleType::Left, 1.0,
                                     angle * Math::Sin(angle) * mult,
                                     angle * Math::Cos(angle) * mult,
                                     0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0,
                                     angle * Math::Cos(angle) * mult,
                                     angle * Math::Sin(angle) * mult,
                                     0.0, MTState::Polymerization);
  }

  parameters->Args->RngSeed = 100500;
  auto res = CudaTest(parameters, CudaCheckBrokenSpring);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/

// Reason: CUDA solver is not yet refactored
/*TEST(Cuda, Series)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();

  parameters->Config[SimParameter::Int::N_Cr_Total] = 2;
  parameters->Config[SimParameter::Int::N_MT_Total] = 200;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;
  parameters->Config[SimParameter::Double::T_End] = 200;

  parameters->Args->CellCount = 5;
  auto res = CudaTest(parameters, CudaCheckNothing);
  if (!String::IsNullOrEmpty(res))
  { FAIL() << StringToString(res); }
}*/
