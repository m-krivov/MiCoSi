#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline SimParams ^SpringConfig(int mtsPerPole)
{
  auto config = gcnew SimParams();
  config[SimParameter::Int::N_MT_Total] = mtsPerPole;
  config[SimParameter::Int::N_Cr_Total] = 1;
  config[SimParameter::Int::Frozen_Coords] = 0;
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

static inline double SpringLength(Vector3 pos1, Vector3 pos2)
{
  return Math::Sqrt((pos1.X - pos2.X) * (pos1.X - pos2.X) +
                    (pos1.Y - pos2.Y) * (pos1.Y - pos2.Y) +
                    (pos1.Z - pos2.Z) * (pos1.Z - pos2.Z));
}

static inline String ^SpringChecker(TimeStream ^ts, Object ^obj)
{
  bool shouldStrech = (bool)obj;
  ts->MoveTo(0);
  if (Enumerable::Count(ts->Current->Cell->Chromosomes) != 2)
  { return "Wrong count of chromosomes"; }

  double expectedLength = ts->Current->SimParams->GetValue(SimParameter::Double::Spring_Length, true);
  double initialLength = SpringLength(Enumerable::ElementAt(ts->Current->Cell->Chromosomes, 0)->Position,
                                      Enumerable::ElementAt(ts->Current->Cell->Chromosomes, 1)->Position);

  ts->MoveTo(ts->LayerCount - 1);
  if (Enumerable::Count(ts->Current->Cell->Chromosomes) != 2)
  { return "Wrong count of chromosomes"; }
  double finalLength = SpringLength(Enumerable::ElementAt(ts->Current->Cell->Chromosomes, 0)->Position,
                                    Enumerable::ElementAt(ts->Current->Cell->Chromosomes, 1)->Position);

  double eps = expectedLength * 1e-3;
  if (Math::Abs(initialLength - expectedLength) > eps)
  { return "Wrong initial spring length"; }
  if (finalLength < initialLength - eps)
  { return "The final spring length cannot be shorter than initial"; }
  if (shouldStrech ^ (Math::Abs(initialLength - finalLength) > eps))
  { return "Wrong size of the spring"; }

  return nullptr;
}

TEST(Spring, Rod)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  double r = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);
  double d = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true) / 2;
  parameters->InitialStates->AddChromosomePair(0.0, d / 2, 0.0, 0.0, 0.0, 0.0);
  parameters->InitialStates->AddMT(PoleType::Left, d / r, d / 2 / r, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -d / r, d / 2 / r, 0.0, 0.0, MTState::Polymerization);

  parameters->Config = SpringConfig(1);
  parameters->Config[SimParameter::Double::Spring_Length] = 0.1;
  parameters->Config[SimParameter::Int::Spring_Type] = 0;
  UNIFIED_TEST(parameters, SpringChecker, false);
}

TEST(Spring, True)
{
  auto parameters = gcnew LaunchParameters();
  parameters->InitialStates = gcnew InitialStates();
  double r = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);
  double d = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true) / 2;
  parameters->InitialStates->AddChromosomePair(0.0, d / 2, 0.0, 0.0, 0.0, 0.0);
  parameters->InitialStates->AddMT(PoleType::Left, d / r, d / 2 / r, 0.0, 0.0, MTState::Polymerization);
  parameters->InitialStates->AddMT(PoleType::Right, -d / r, d / 2 / r, 0.0, 0.0, MTState::Polymerization);

  parameters->Config = SpringConfig(1);
  parameters->Config[SimParameter::Double::Spring_Length] = 0.1;
  parameters->Config[SimParameter::Int::Spring_Type] = 1;
  parameters->Config[SimParameter::Double::Spring_K] = 20;
  UNIFIED_TEST(parameters, SpringChecker, true);
}

static inline String ^SpringHookesLawChecker(TimeStream ^ts, Object ^obj)
{
  ts->Reset();
  if (!ts->MoveNext()) { "No time layers"; }
  double const_a = ts->Current->SimParams->GetValue(SimParameter::Double::Const_A);
  double spring_k = ts->Current->SimParams->GetValue(SimParameter::Double::Spring_K);
  double length_start = ts->Current->SimParams->GetValue(SimParameter::Double::Spring_Length) / 10e+5;

  int doneChecks = 0, prev_mt_count = -1;
  ts->Reset();
  while (ts->MoveNext())
  {
    auto cell = ts->Current->Cell;
    if (Enumerable::Count(cell->Chromosomes) != 2) { return "Wrong count of chromosomes"; }
    auto chr0 = Enumerable::ElementAt(cell->Chromosomes, 0);
    auto chr1 = Enumerable::ElementAt(cell->Chromosomes, 1);

    int mt_count = Enumerable::Count(chr0->BoundMTs) + Enumerable::Count(chr1->BoundMTs);
    if (mt_count == prev_mt_count&& mt_count > 0)
    {
      double delta = Math::Abs(SpringLength(chr0->Position, chr1->Position) - length_start);
      double actual_k = mt_count * const_a / (delta * 1e6) / 2;
      if (Math::Abs(actual_k - spring_k) > 1.0)
      { return "Wrong actual K-coefficient"; }
      doneChecks += 1;
    }
    prev_mt_count = mt_count;
  }

  if (doneChecks < 50)
  { return "Not enough checks"; }

  return nullptr;
}

static inline LaunchParameters ^SpringParameters(double k_on, double spring_k)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Config = SpringConfig(33);
  parameters->Config[SimParameter::Int::Frozen_Coords] = 1;
  parameters->Config[SimParameter::Double::Spring_K] = spring_k;
  parameters->Config[SimParameter::Double::Const_B] = 1;
  parameters->Config[SimParameter::Int::Spring_Type] = 1;
  parameters->Config[SimParameter::Double::Spring_Length] = 2e-1;
  parameters->Config[SimParameter::Double::V_Dep] = 5.0;
  parameters->Config[SimParameter::Double::V_Pol] = 80.0;
  parameters->Config[SimParameter::Double::K_On] = k_on;
  parameters->Config[SimParameter::Double::Gamma]
    = SimParams::GetDefaultValue(SimParameter::Double::Gamma, false) * 1e3;
  parameters->Config[SimParameter::Double::Ieta]
    = SimParams::GetDefaultValue(SimParameter::Double::Ieta, false) * 1e3;

  parameters->InitialStates = gcnew InitialStates();
  parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  for (int i = 0; i < 33; i++)
  {
    double alpha = Math::PI * (16 - i) / 32.0 / 90.0;
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, Math::Sin(alpha), 0.0,
                                     5 * 1e-6, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, Math::Sin(alpha), 0.0,
                                     1e-6, MTState::Polymerization);
  }

  return parameters;
}

// Reason: model is not ready for such iteractions
/*TEST(Spring, HookesLaw_80_50)
{
  auto parameters = SpringParameters(80 / 0.1, 50);
  UNIFIED_TEST(parameters, SpringHookesLawChecker, 50);
}*/

TEST(Spring, HookesLaw_40_500)
{
  auto parameters = SpringParameters(40 / 0.1, 500);
  UNIFIED_TEST(parameters, SpringHookesLawChecker, 500);
}
