#pragma once
#include "Defs.h"
#include "Helpers.h"

TEST(RNG, UserSeed)
{
  Helper::PrepareTestDirectory();
  
  cli::array<IntPtr> ^data = gcnew cli::array<IntPtr>{ (IntPtr)nullptr, (IntPtr)nullptr, (IntPtr)nullptr };
  try
  {
    TimeStream ^ts = nullptr;
    auto parameters = gcnew LaunchParameters();
    parameters->Config = gcnew SimParams();
    parameters->Args->Mode = LaunchMode::New;

    // Ensure that seed is stored
    parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
    parameters->Config[SimParameter::Double::T_End] = 2.0;
    parameters->Args->UserSeed = 100500;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ASSERT_EQ(ts->UserSeed, 100500);
    }
    finally { if (ts != nullptr) { delete ts; ts = nullptr; } }

    // Ensure that seeds are not ignored
    parameters->Config[SimParameter::Int::N_Cr_Total] = 3;
    parameters->Config[SimParameter::Double::T_End] = 10.0;
    
    for (int i = 0; i < data->Length; i++)
    {
      parameters->Args->UserSeed = (i == 2 ? 200 : 100);
      try
      {
        ts = Helper::LaunchAndOpen(parameters);
        ts->MoveTo(ts->LayerCount - 1);
        data[i] = Helper::CopyData(ts->Current->Cell);
      }
      finally { if (ts != nullptr) { delete ts; ts = nullptr; } }
    }

    if (!Helper::CompareData(data[0], data[1]))
    { FAIL() << "The same user seed provides different results"; }

    if (Helper::CompareData(data[0], data[2]))
    { FAIL() << "Different user seeds provide the same results"; }
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
  finally
  {
    for each (auto r in data)
    {
      if (r != (IntPtr)nullptr)
      { Helper::ReleaseData(r); }
    }
    Helper::ClearUpTestDirectory();
  }
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

    // Get data for original launch
    TimeStream ^ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      started = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) { delete ts; } }

    // Get data for restarted launch
    parameters->Args->Mode = LaunchMode::Restart;
    ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      restarted = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) { delete ts; } }

    // Check
    if (!Helper::CompareData(started, restarted))
    { FAIL() << "Not equal results"; }
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
  finally
  {
    if (started != (IntPtr)nullptr) { Helper::ReleaseData(started); }
    if (restarted != (IntPtr)nullptr) { Helper::ReleaseData(restarted); }
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

    // Get data for single launch
    TimeStream ^ts = nullptr;
    try
    {
      ts = Helper::LaunchAndOpen(parameters);
      ts->MoveTo(ts->LayerCount - 1);
      started = Helper::CopyData(ts->Current->Cell);
    }
    finally { if (ts != nullptr) delete ts; }
        
    // Get data for multiple launches
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

    // Check
    if (!Helper::CompareData(started, continued))
    { FAIL() << "Not equal results"; }
  }
  catch (Exception ^ex)
  { FAIL() << StringToString(ex->Message); }
  finally
  {
    if (started != (IntPtr)nullptr) { Helper::ReleaseData(started); }
    if (continued != (IntPtr)nullptr) { Helper::ReleaseData(continued); }
    Helper::ClearUpTestDirectory();
  }
}
