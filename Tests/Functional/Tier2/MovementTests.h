#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline SimParams ^MovementConfig(int mtsPerPole, bool jumping)
{
  auto config = gcnew SimParams();
      
  config[SimParameter::Int::N_Cr_Total] = 1;
  config[SimParameter::Int::N_MT_Total] = mtsPerPole;
  config[SimParameter::Int::Frozen_Coords] = 0;
  config[SimParameter::Double::F_Cat] = 0.0;
  config[SimParameter::Double::V_Dep] = 0.0;
  config[SimParameter::Double::K_On] = 10.0;
  config[SimParameter::Double::K_Off] = 0.0;
  config[SimParameter::Double::D_Rot] = 0.0;
  config[SimParameter::Double::D_Trans] = 0.0;
  config[SimParameter::Double::Spring_Brake_Force] = 100500 * 1e3;
  config[SimParameter::Double::T_End] = 1000.0;
  if (jumping)
  {
    config[SimParameter::Double::Ieta] = SimParams::GetDefaultValue(SimParameter::Double::Ieta, false) * 1e-3;
    config[SimParameter::Double::Const_A] = SimParams::GetDefaultValue(SimParameter::Double::Const_A, false) * 1e3;
  }

  return config;
}

static inline LaunchParameters ^MovementConfigure(bool jumping)
{
  auto parameters = gcnew LaunchParameters();
  double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);
  double kin_d = SimParams::GetDefaultValue(SimParameter::Double::Cr_Kin_D, true);
  double kin_l = SimParams::GetDefaultValue(SimParameter::Double::Cr_Kin_L, true);
  double normalizer = 1.0 / l_poles;

  parameters->Config = MovementConfig(20, jumping);

  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  for (int i = 0; i < parameters->Config[SimParameter::Int::N_MT_Total]; i++)
  {
    parameters->InitialStates->AddMT(PoleType::Left, l_poles / 2 * normalizer,
                                     -kin_d * ((i % 4) + 1) / 5.0 * normalizer / 2,
                                     0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -l_poles / 2 * normalizer,
                                     kin_l * ((i % 6) + 1) / 5.0 * normalizer,
                                     0.0, 0.0, MTState::Polymerization);
  }

  return parameters;
}

static inline String ^MovementFrozenCoordsChecker(TimeStream ^ts, Object ^obj)
{
  double eps = (double)obj;
  Vec3 ^pos = nullptr;

  if (ts->LayerCount < 2)
  { return "Broken test: not enough time layers"; }

  ts->Reset();
  while (ts->MoveNext())
  {
    auto cell = ts->Current->Cell;
    if (Enumerable::Count(cell->Chromosomes) != 2)
    { return "Broken test: the number of chromosomes must be equal to two"; }

    if (pos == nullptr)
    {
      pos = (gcnew Vec3(Enumerable::ElementAt(cell->Chromosomes, 0)->Position) +
             gcnew Vec3(Enumerable::ElementAt(cell->Chromosomes, 1)->Position)) / 2;
    }
    else
    {
      auto pos1 = gcnew Vec3(Enumerable::ElementAt(cell->Chromosomes, 0)->Position);
      auto pos2 = gcnew Vec3(Enumerable::ElementAt(cell->Chromosomes, 1)->Position);
      auto newPos = (pos1 + pos2) / 2;

      auto cEps = (newPos - pos)->Length;
      if (cEps > eps)
      { return "The frozen coordinate option is not working properly"; }
    }
  }

  return nullptr;
}

TEST(Movement, FrozenCoords)
{
  double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);
  auto parameters = MovementConfigure(false);
      
  parameters->PoleCoords = gcnew PoleCoordinates();
  parameters->PoleCoords->AddRecord(0.0, -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(35.0, -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(60.0, -l_poles / 3, l_poles / 6, 0.0, l_poles / 2, 0.0, l_poles / 5);
      
  parameters->Config[SimParameter::Int::Frozen_Coords] = 1;
  UNIFIED_TEST(parameters, MovementFrozenCoordsChecker,
               SimParams::GetDefaultValue(SimParameter::Double::Cr_Kin_D, true) * 1e-2);
}

static inline String ^MovementYZChecker(TimeStream ^ts, Object ^epsObj)
{
  double eps = (double)epsObj;
  ts->Reset();
  while (ts->MoveNext())
  {
    auto cell = ts->Current->Cell;
    auto chr1 = Enumerable::ElementAt(cell->Chromosomes, 0);
    auto chr2 = Enumerable::ElementAt(cell->Chromosomes, 1);
    Vec3 ^pos = (gcnew Vec3(chr1->Position) + gcnew Vec3(chr2->Position)) / 2;
    if (Math::Abs(pos->x) > eps)
    { return "X-movement detected"; }
  }

  return nullptr;
}

static inline LaunchParameters ^MovementConfigureYZ(bool jumping)
{
  double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);
  auto parameters = MovementConfigure(jumping);

  parameters->PoleCoords = gcnew PoleCoordinates();
  parameters->PoleCoords->AddRecord(0.0, -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
  parameters->PoleCoords->AddRecord(35.0, -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
  for (int i = 0; i < 100; i++)
  {
    double ni = i / 99.0;
    double alpha = 2 * Math::PI * ni * 6;
    parameters->PoleCoords->AddRecord(35.0 + (i + 1) * 5.0,
                                      -l_poles / 2, l_poles / 4 * Math::Sin(alpha), l_poles / 4 * Math::Cos(alpha),
                                      l_poles / 2, l_poles / 4 * Math::Sin(alpha), l_poles / 4 * Math::Cos(alpha));
  }

  parameters->Config[SimParameter::Int::Frozen_Coords] = 0;
  return parameters;
}

TEST(Movement, YZ_Stable)
{
  auto parameters = MovementConfigureYZ(false);
  UNIFIED_TEST(parameters, MovementYZChecker,
               SimParams::GetDefaultValue(SimParameter::Double::Cr_Kin_D, true) * 1e-2);
}

TEST(Movement, YZ_Jumping)
{
  auto parameters = MovementConfigureYZ(true);
  UNIFIED_TEST(parameters, MovementYZChecker,
               SimParams::GetDefaultValue(SimParameter::Double::Cr_Kin_D, true) * 1e-2);
}
