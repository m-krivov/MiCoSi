#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline String ^RngSeedChecker(TimeStream ^ts, Object ^obj)
{
  UInt32 seed = (UInt32)obj;
  ts->MoveTo(0);
  if (ts->InitialRandSeed != seed)
  { return "Specified RNG seed was ignored"; }

  return nullptr;
}

TEST(RNG, ExplicitSeed)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_Cr_Total] = 23;
  parameters->Config[SimParameter::Int::N_MT_Total] = 1500;
  parameters->Config[SimParameter::Double::Dt] = 1.0;
  parameters->Config[SimParameter::Double::T_End] = 2.0;
  parameters->Args->RngSeed = 100500;

  UNIFIED_TEST(parameters, RngSeedChecker, (Object ^)parameters->Args->RngSeed);
}

TEST(RNG, StartRestart)
{
  IntPtr started = (IntPtr)nullptr, restarted = (IntPtr)nullptr;
  Helper::PrepareTestDirectory();

  try
  {
    auto parameters = gcnew LaunchParameters();
    parameters->Config = gcnew SimParams();
    parameters->Config[SimParameter::Int::N_Cr_Total] = 3;
    parameters->Config[SimParameter::Int::N_MT_Total] = 750;
    parameters->Config[SimParameter::Double::Dt] = 0.5;
    parameters->Config[SimParameter::Double::T_End] = 10.5;
    parameters->Args->Mode = LaunchMode::New;

    //Getting data for original launch.
    TimeStream ^ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      started = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) { delete ts; } }

    //Getting data for restarted launch.
    parameters->Args->Mode = LaunchMode::Restart;
    ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      restarted = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) { delete ts; } }

    //Checking.
    if (!Helper::CompareData(started, restarted))
    { FAIL() << "Not equal results"; }
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
  finally
  {
    if (started != (IntPtr)nullptr) Helper::ReleaseData(started);
    if (restarted != (IntPtr)nullptr) Helper::ReleaseData(restarted);
    Helper::ClearUpTestDirectory();
  }
}

TEST(RNG, StartContinue)
{
  IntPtr started = (IntPtr)nullptr, continued = (IntPtr)nullptr;
  Helper::PrepareTestDirectory();

  try
  {
    auto parameters = gcnew LaunchParameters();
    parameters->Config = gcnew SimParams();
    parameters->Config[SimParameter::Int::N_Cr_Total] = 5;
    parameters->Config[SimParameter::Int::N_MT_Total] = 500;
    parameters->Config[SimParameter::Double::Dt] = 0.1;
    parameters->Config[SimParameter::Double::T_End] = 1.0;
    parameters->Args->Mode = LaunchMode::New;

    //Getting data for single launch.
    TimeStream ^ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      started = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) delete ts; }
        
    //Getting data for multiple launches.
    parameters->Config[SimParameter::Double::T_End] = 0.5;
    parameters->Args->Mode = LaunchMode::Restart;
    delete Helper::LaunchAndOpen(parameters);
    parameters->Args->Mode = LaunchMode::Continue;
    for (int i = 1; i < 5; i++)
    {
      parameters->Config[SimParameter::Double::T_End] = Math::Round(0.5 + i * 0.1, 1);
      delete Helper::LaunchAndOpen(parameters);
    }

    parameters->Config[SimParameter::Double::T_End] = 1.0;
    ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      continued = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) { delete ts; } }

    //Checking.
    if (!Helper::CompareData(started, continued))
    { FAIL() << "Not equal results"; }
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
  finally
  {
    if (started != (IntPtr)nullptr) Helper::ReleaseData(started);
    if (continued != (IntPtr)nullptr) Helper::ReleaseData(continued);
    Helper::ClearUpTestDirectory();
  }
}
