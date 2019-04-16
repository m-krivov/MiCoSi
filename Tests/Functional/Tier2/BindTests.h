#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline SimParams ^MtBindingConfig(int mtsPerPole)
{
  auto config = gcnew SimParams();
  config[SimParameter::Int::N_MT_Total] = mtsPerPole;
  config[SimParameter::Int::N_Cr_Total] = 1;
  config[SimParameter::Int::Frozen_Coords] = 1;
  config[SimParameter::Double::Dt] = 0.1;
  config[SimParameter::Double::T_End] = 100.0;
  config[SimParameter::Double::Spring_Length] = 0.0;
  config[SimParameter::Double::K_On] = 1.0 / 0.1;
  config[SimParameter::Double::K_Off] = 0.0;
  config[SimParameter::Double::Spring_Brake_Force] = 100500;
  config[SimParameter::Int::Spring_Brake_MTs] = 100500;
  config[SimParameter::Double::D_Trans] = 0.0;
  config[SimParameter::Double::D_Rot] = 0.0;
  config[SimParameter::Double::F_Cat] = 0.0;
  return config;
}

static inline String ^MtBindingChecker(TimeStream ^ts, Object ^obj)
{
  bool shouldBind = (bool)obj;
  ts->MoveTo(ts->LayerCount - 1);
  auto cell = ts->Current->Cell;

  for each (auto mt in cell->MTs)
  {
    if (shouldBind ^ (mt->BoundChromosome != nullptr))
    { return gcnew String("Some MTs are not bound"); }
  }

  return nullptr;
}

TEST(MtBinding, Center)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.0, 0.0, 0.0, MTState::Polymerization);

  parameters->Config = MtBindingConfig(1);
  UNIFIED_TEST(parameters, MtBindingChecker, true);
}

TEST(MtBinding, NotCenter)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0000033, 0.0, 0.0, 0.0, 0.0);
  parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.5025, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.49025, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.5025, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.49025, 0.0, 0.0, MTState::Polymerization);

  parameters->Config = MtBindingConfig(2);
  UNIFIED_TEST(parameters, MtBindingChecker, true);
}

TEST(MtBinding, SpringGap)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, Math::PI / 2, 0.0);
  for (int i = 0; i < 10; i++)
  {
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.5 - i / 9.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, i / 9.0 - 0.5, 0.0, 0.0, MTState::Polymerization);
  }

  parameters->Config = MtBindingConfig(10);
  parameters->Config[SimParameter::Double::Spring_Length] = 2e-1;
  UNIFIED_TEST(parameters, MtBindingChecker, false);
}

TEST(MtBinding, MovingChromosome)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  double r = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);
  double p = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true) / 2;
  parameters->InitialStates->AddChromosomePair(0.0, r / 2, 0.0, Math::PI / 2, 0.0, 0.0);
  parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.1, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Left, 1e6 * p, 1e6 * r / 2, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.1, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1e6 * p, 1e6 * r / 2, 0.0, 0.0, MTState::Polymerization);

  parameters->Config = MtBindingConfig(2);
  parameters->Config[SimParameter::Int::Frozen_Coords] = 0;
  parameters->Config[SimParameter::Double::T_End] = 60.0;
  UNIFIED_TEST(parameters, MtBindingChecker, true);
}

TEST(MtBinding, MovingPoles)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  double L_Poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);
  parameters->InitialStates->AddChromosomePair(0.0f, L_Poles / 2 / 5, 0.0, Math::PI / 2, 0.0, 0.0);
  parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.1, 0.0, L_Poles, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Left, 1.0, -0.1, 0.0, L_Poles, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.1, 0.0, L_Poles, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -1.0, -0.1, 0.0, L_Poles, MTState::Polymerization);

  parameters->Config = MtBindingConfig(2);
  parameters->Config[SimParameter::Int::Frozen_Coords] = 1;
  parameters->Config[SimParameter::Double::V_Dep] = 0.0;

  parameters->PoleCoords = gcnew PoleCoordinates();
  parameters->PoleCoords->AddRecord(0.0, -L_Poles / 2, 0.0, 0.0, L_Poles / 2, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(50.0, -L_Poles / 2, L_Poles / 2 / 2, 0.0, L_Poles / 2, L_Poles / 2 / 2, 0.0);
  UNIFIED_TEST(parameters, MtBindingChecker, true);
}

static inline String ^MtBindingForcePointChecker(TimeStream ^ts, Object ^hasForcePoints)
{
  bool hasFP = (bool)hasForcePoints;

  ts->Reset();
  ts->MoveTo(ts->LayerCount - 1);
  auto cell = ts->Current->Cell;

  int boundMTs = 0;
  for each (auto mt in cell->MTs)
  {
    if (mt->BoundChromosome != nullptr)
    {
      bool isForcePoint = mt->EndPoint->X != mt->ForcePoint->X ||
                mt->EndPoint->Y != mt->ForcePoint->Y ||
                mt->EndPoint->Z != mt->ForcePoint->Z;
      if (isForcePoint ^ hasFP)
        return gcnew String(hasFP ? "Some force points were missed"
                                  : "Some false force points were detected");
      boundMTs += 1;
    }
  }

  if (boundMTs < 50)
  { return gcnew String("Unit test is broken"); }

  return nullptr;
}

static inline LaunchParameters ^MtBindingForcePointParams(int mts)
{
  auto parameters = gcnew LaunchParameters();
      
  parameters->Config = MtBindingConfig(mts);
  parameters->Config[SimParameter::Double::V_Dep] = 0.0;
  parameters->Config[SimParameter::Double::Const_A] = 0.0;
  parameters->Config[SimParameter::Double::Const_B] = 0.0;
  parameters->Config[SimParameter::Double::Spring_Length] = 0.1;

  parameters->InitialStates = gcnew InitialStates();
  for (int i = 0; i < mts; i++)
  {
    double ni = (double)i / (mts - 1);
    double angle = ni * 8 * 2 * Math::PI;
    double r_step = 0.03;
    parameters->InitialStates->AddMT(PoleType::Left,
                                     1.0, Math::Sin(angle) * ni * r_step, Math::Cos(angle) * ni * r_step,
                                     0.0,
                                     MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right,
                                     -1.0, Math::Cos(angle) * ni * r_step, Math::Sin(angle) * ni * r_step,
                                     0.0,
                                     MTState::Polymerization);
  }

  return parameters;
}

TEST(MtBinding, NoForcePoints)
{
  auto parameters = MtBindingForcePointParams(200);
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  UNIFIED_TEST(parameters, MtBindingForcePointChecker, false);
}

TEST(MtBinding, HasForcePoints)
{
  auto parameters = MtBindingForcePointParams(200);
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);
  parameters->PoleCoords = gcnew PoleCoordinates();
  parameters->PoleCoords->AddRecord(0.0, -l_poles / 4, 0.0, 0.0, l_poles / 4, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(30.0, -l_poles / 4, 0.0, 0.0, l_poles / 4, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(60.0, l_poles / 4, 0.0, 0.0, -l_poles / 4, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(90.0, l_poles / 3, l_poles / 5, 0.0, -l_poles / 3, l_poles / 3, 0.0);

  UNIFIED_TEST(parameters, MtBindingForcePointChecker, true);
}
