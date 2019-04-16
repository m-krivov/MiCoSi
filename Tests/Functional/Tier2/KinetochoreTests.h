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
      auto orient = gcnew Matrix3x3(chr->Orientation);
      auto ortY = orient->Rotate(gcnew Vec3(0, 1, 0))->Normailzed;
      auto ortX = orient->Rotate(gcnew Vec3(1, 0, 0))->Normailzed;

      auto dp = gcnew Vec3(mt->EndPoint) - gcnew Vec3(chr->Position);
      dp = dp - ortY * Vec3::DotProduct(dp, ortY);
      double cur_angle = Math::Acos(Vec3::DotProduct(dp->Normailzed, ortX)) / Math::PI * 180.0;
      if (cur_angle > kin_angle)
      { return "Wrong kinetochore angle"; }

      boundMTs += 1;
    }
  }

  if (boundMTs < 30)
  { return "Broken unit test - not enought MTs"; }

  return nullptr;
}

static inline LaunchParameters ^KinetochoreConfigure(double angle)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_Cr_Total] = 1;
  parameters->Config[SimParameter::Int::N_MT_Total] = 500;
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
    auto res = gcnew Vec3(1.0,
                          ni * 1.0 / 16 * Math::Sin(ni * 8 * 2 * Math::PI),
                          ni * 1.0 / 16 * Math::Cos(ni * 8 * 2 * Math::PI));
    res = res->Normailzed;
    parameters->InitialStates->AddMT(PoleType::Left, res->x, res->y, res->z, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -res->x, res->z, -res->y, 0.0, MTState::Polymerization);
  }
  return parameters;
}

TEST(Kinetochore, Angle133)
{
  auto parameters = KinetochoreConfigure(133);
  UNIFIED_TEST(parameters, KinetochoreAngleChecker, nullptr);
}

TEST(Kinetochore, Angle42)
{
  auto parameters = KinetochoreConfigure(42);
  UNIFIED_TEST(parameters, KinetochoreAngleChecker, nullptr);
}
