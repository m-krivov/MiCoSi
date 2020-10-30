#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline String ^KinetochoreAngleChecker(TimeStream ^ts, Object ^arg)
{
  if (ts->LayerCount <= 10) return "Broken test - not enough time layers";

  ts->Reset();
  ts->MoveTo(ts->LayerCount - 1);
  double kin_angle = ts->Current->SimParams->GetValue(SimParameter::Double::Cr_Kin_Angle, false) / 2;  //degrees
  auto cell = ts->Current->Cell;
  int boundMTs = 0;
  for each (auto mt in cell->MTs)
  {
    if (mt->BoundChromosome != nullptr)
    {
      auto chr = mt->BoundChromosome;
      auto orient = Matrix3x3(chr->Orientation);
      auto ortY = Vector3::Normalize(orient * Vector3(0, 1, 0));
      auto ortX = Vector3::Normalize(orient * Vector3(1, 0, 0));

      auto dp = mt->EndPoint + (-chr->Position);
      dp = dp + (-ortY * Vector3::Dot(dp, ortY));
      double cur_angle = Math::Acos(Vector3::Dot(Vector3::Normalize(dp), ortX)) / Math::PI * 180.0;
      if (cur_angle > kin_angle)
      {
        return String::Format("Wrong kinetochore angle (Current: {0}, Cr_Kin_Angle: {1})",
                              cur_angle, kin_angle);
      }

      boundMTs += 1;
    }
  }

  if (boundMTs < 30)
  { return "Broken unit test - not enought MTs"; }

  return nullptr;
}

static inline LaunchParameters ^KinetochoreAngleConfigure(double angle)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
  parameters->Config[SimParameter::Int::N_MT_Total] = 500;
  parameters->Config[SimParameter::Int::N_KMT_Max] = 1000;
  parameters->Config[SimParameter::Double::V_Dep] = 0.0;
  parameters->Config[SimParameter::Double::F_Cat] = 0.0;
  parameters->Config[SimParameter::Double::Cr_Kin_Angle] = angle;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;
  parameters->Config[SimParameter::Double::Const_A] = 0.0;
  parameters->Config[SimParameter::Double::Const_B] = 0.0;
  parameters->Config[SimParameter::Double::K_On] = 10.0;
  parameters->Config[SimParameter::Double::K_Off] = 0.0;
  parameters->Config[SimParameter::Double::Spring_Length] = 0.004;

  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.07, Math::PI / 1.05, 0.1);

  int mts = parameters->Config[SimParameter::Int::N_MT_Total];
  for (int i = 0; i < mts; i++)
  {
    double ni = (double)i / (mts - 1);
    auto res = Vector3(1.0f,
                       (float)(ni * 1.0 / 16 * Math::Sin(ni * 8 * 2 * Math::PI)),
                       (float)(ni * 1.0 / 16 * Math::Cos(ni * 8 * 2 * Math::PI)));
    res = Vector3::Normalize(res);
    parameters->InitialStates->AddMT(PoleType::Left, res.X, res.Y, res.Z, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -res.X, res.Z, -res.Y, 0.0, MTState::Polymerization);
  }
  return parameters;
}

TEST(Kinetochore, Angle133)
{
  auto parameters = KinetochoreAngleConfigure(133);
  UNIFIED_TEST(parameters, KinetochoreAngleChecker, nullptr);
}

TEST(Kinetochore, Angle42)
{
  auto parameters = KinetochoreAngleConfigure(42);
  UNIFIED_TEST(parameters, KinetochoreAngleChecker, nullptr);
}


static inline LaunchParameters ^KinetochoreKMTConfigure(int n_mt, int n_kmt_max)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
  parameters->Config[SimParameter::Int::N_MT_Total] = n_mt;
  parameters->Config[SimParameter::Int::N_KMT_Max] = n_kmt_max;
  parameters->Config[SimParameter::Double::V_Dep] = 0.0;
  parameters->Config[SimParameter::Double::F_Cat] = 0.0;
  parameters->Config[SimParameter::Double::Cr_Kin_Angle] = 180.0;
  parameters->Config[SimParameter::Double::D_Rot] = 0.0;
  parameters->Config[SimParameter::Double::D_Trans] = 0.0;
  parameters->Config[SimParameter::Double::Const_A] = 0.0;
  parameters->Config[SimParameter::Double::Const_B] = 0.0;
  parameters->Config[SimParameter::Double::K_On] = 10.0;
  parameters->Config[SimParameter::Double::K_Off] = 0.0;
  parameters->Config[SimParameter::Double::Spring_Length] = 0.004;

  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  for (int i = 0; i < n_mt; i++)
  {
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
  }
  return parameters;
}

static inline String ^KinetochoreKMTChecker(TimeStream ^ts, Object ^arg)
{
  int kmt_max = -1;
  int kmt_left = 0, kmt_right = 0;
  while (ts->MoveNext())
  {
    if (kmt_max < 0)
    {
      kmt_max = Math::Min(ts->Current->SimParams->GetValue(SimParameter::Int::N_KMT_Max),
                          ts->Current->SimParams->GetValue(SimParameter::Int::N_MT_Total));
    }

    auto cell = ts->Current->Cell;
    auto mts = cell->MTs;

    kmt_left = kmt_right = 0;
    for each (auto mt in mts)
    {
      if (mt->BoundChromosome != nullptr)
      {
        if (mt->Pole->Type == PoleType::Left)
        { kmt_left += 1; }
        else
        { kmt_right += 1; }
      }
    } // for each mt

    if (kmt_left > kmt_max || kmt_right > kmt_max)
    { return "Actual KMTs exceeded the maximum permissible value"; }
  } // while

  if (kmt_left != kmt_max || kmt_right != kmt_max)
  { return "Not enough KMTs at the end of simulation"; }

  return nullptr;
}

TEST(Kinetochore, KMTs_100_10)
{
  auto parameters = KinetochoreKMTConfigure(100, 10);
  UNIFIED_TEST(parameters, KinetochoreKMTChecker, nullptr);
}

TEST(Kinetochore, KMTs_10_100)
{
  auto parameters = KinetochoreKMTConfigure(10, 100);
  UNIFIED_TEST(parameters, KinetochoreKMTChecker, nullptr);
}

TEST(Kinetochore, KMTs_42_42)
{
  auto parameters = KinetochoreKMTConfigure(42, 42);
  UNIFIED_TEST(parameters, KinetochoreKMTChecker, nullptr);
}
