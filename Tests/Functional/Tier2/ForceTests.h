#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline String ^ForceDistanceChecker(TimeStream ^ts, Object ^epsObj)
{
  double eps = (double)epsObj;

  ts->Reset();
  double prevDist = Double::MaxValue;
  while (ts->MoveNext())
  {
    auto cell = ts->Current->Cell;
    if (cell->AreSpringsBroken)
    { return "Springs are broken. But they should not be broken!"; }

    auto chr = Enumerable::First(cell->ChromosomePairs);
    {
      if (Enumerable::Count(chr->LeftChromosome->BoundMTs) == 1 &&
          Enumerable::Count(chr->RightChromosome->BoundMTs) == 1)
      {
        auto mt_l = Enumerable::First(chr->LeftChromosome->BoundMTs);
        auto mt_r = Enumerable::First(chr->RightChromosome->BoundMTs);
        double dist = mt_l->Length + mt_r->Length;
        if (prevDist < dist - eps)
        { return "Chromosomes are moving away from the equilibrium point"; }
        prevDist = dist;
      }
    }
  } // while ts

  ts->MoveTo(ts->LayerCount - 1);
  {
    auto cell = ts->Current->Cell;
    double dist1 = (cell->GetPole(PoleType::Left)->Position +
                    (-cell->GetPole(PoleType::Right)->Position)).Length();

    double dist2 = (cell->GetPole(PoleType::Left)->Position +
                    (-Enumerable::First(cell->ChromosomePairs)->LeftChromosome->Position)).Length() +
                   (cell->GetPole(PoleType::Right)->Position +
                    (-Enumerable::First(cell->ChromosomePairs)->RightChromosome->Position)).Length();
    if (Math::Abs(1.0 - dist2 / dist1) > 0.1)
    { return "Chromosomes are not centered!"; }
  }

  return nullptr;
}

static inline String ^ForceOrientationChecker(TimeStream ^ts, Object ^epsObj)
{
  double eps = (double)epsObj;

  ts->Reset();
  double prevCos = Double::MinValue;
  while (ts->MoveNext())
  {
    auto cell = ts->Current->Cell;
    if (cell->AreSpringsBroken)
    { return "Springs are broken. But they should not be broken!"; }

    auto chr = Enumerable::First(cell->ChromosomePairs);
    {
      if (Enumerable::Count(chr->LeftChromosome->BoundMTs) == 1 &&
          Enumerable::Count(chr->RightChromosome->BoundMTs) == 1)
      {
        auto orient_l = Matrix3x3(chr->LeftChromosome->Orientation);
        auto orient_r = Matrix3x3(chr->RightChromosome->Orientation);
        auto dir_l = Vector3::Normalize(chr->LeftChromosome->Position +
                                        (-cell->GetPole(PoleType::Left)->Position));
        auto dir_r = Vector3::Normalize(chr->RightChromosome->Position +
                                       (-cell->GetPole(PoleType::Right)->Position));

        double cos = Math::Min(Vector3::Dot(dir_l, orient_l * Vector3(-1.0f, 0.0f, 0.0f)),
                               Vector3::Dot(dir_r, orient_r * Vector3(-1.0f, 0.0f, 0.0f)));

        if (cos + eps < prevCos)
        { return "Chromosomes were rotated in wrong direction"; }
        prevCos = cos;
      }
    }
  }

  ts->MoveTo(ts->LayerCount - 1);
  {
    auto cell = ts->Current->Cell;
    auto chr = Enumerable::First(cell->ChromosomePairs);

    auto orient_l = Matrix3x3(chr->LeftChromosome->Orientation);
    auto orient_r = Matrix3x3(chr->RightChromosome->Orientation);
    auto dir_l = Vector3::Normalize(chr->LeftChromosome->Position +
                                    (-cell->GetPole(PoleType::Left)->Position));
    auto dir_r = Vector3::Normalize(chr->RightChromosome->Position +
                                    (-cell->GetPole(PoleType::Right)->Position));

    if (Math::Abs(Vector3::Dot(dir_l, orient_l * Vector3(0.0f, 1.0f, 0.0f))) > 0.05f ||
        Math::Abs(Vector3::Dot(dir_r, orient_r * Vector3(0.0f, 1.0f, 0.0f))) > 0.05f)
    { return "Chromosomes were rotated in wrong way (Y-dimension is not perpendicular to pole axis)"; }

    if (Vector3::Dot(dir_l, orient_l * Vector3(-1.0f, 0.0f, 0.0f)) < 0.95f ||
        Vector3::Dot(dir_r, orient_r * Vector3(-1.0f, 0.0f, 0.0f)) < 0.95f)
    { return "Chromosomes were rotated in wrong way (X-dimension is not parallel to pole axis)"; }
  }

  return nullptr;
}

static inline String ^ForceDistanceAndOrientationChecker(TimeStream ^ts, Object ^epsObj)
{
  double eps = (double)epsObj;

  ts->Reset();
  Vector3 position;
  Vector3 direction;
  ts->MoveTo(0);
  {
    auto cell = ts->Current->Cell;
    auto chr = Enumerable::First(cell->ChromosomePairs);
    position = (chr->RightChromosome->Position + chr->LeftChromosome->Position) / 2;
  }

  bool isBound = false;
  while (!isBound && ts->MoveNext())
  {
    auto cell = ts->Current->Cell;
    auto chr = Enumerable::First(cell->ChromosomePairs);

    if (Enumerable::Count(chr->LeftChromosome->BoundMTs) == 1 &&
        Enumerable::Count(chr->RightChromosome->BoundMTs) == 1)
    {
      isBound = true;
      auto mt_l = Enumerable::First(chr->LeftChromosome->BoundMTs);
      auto mt_r = Enumerable::First(chr->RightChromosome->BoundMTs);
      direction = Vector3::Normalize(mt_r->ForcePoint + (-mt_l->ForcePoint));
    }
  }

  if (!isBound)
  { return "Some MTs are not bound"; }

  ts->MoveTo(ts->LayerCount - 1);
  {
    auto cell = ts->Current->Cell;
    auto chr = Enumerable::First(cell->ChromosomePairs);

    auto mt_l = Enumerable::First(chr->LeftChromosome->BoundMTs);
    auto mt_r = Enumerable::First(chr->RightChromosome->BoundMTs);
    auto mts_dir = Vector3::Normalize(mt_l->ForcePoint + (-mt_r->ForcePoint));
    auto poles_dir = Vector3::Normalize(cell->GetPole(PoleType::Left)->Position +
                                        (-cell->GetPole(PoleType::Right)->Position));

    auto newPos = (chr->RightChromosome->Position + chr->LeftChromosome->Position) / 2;
    if ((newPos + (-position)).Length() > eps)
    { return "The center of chromosome pair was moved"; }

    double angle = Math::Acos(Vector3::Dot(mts_dir, poles_dir));
    double angle2 = Math::Acos(Vector3::Dot(mts_dir, direction));
    if (Math::Abs(angle) > 0.05 || Math::Abs(angle2) > 0.05)
    { return "Chromosomes have changed orientation"; }
  }

  return nullptr;
}

static inline SimParams ^ForceConfig()
{
  auto config = gcnew SimParams();

  config[SimParameter::Int::N_Cr_Total] = 1;
  config[SimParameter::Int::N_MT_Total] = 1;
  config[SimParameter::Int::Frozen_Coords] = 0;
  config[SimParameter::Double::K_On] = 10.0;
  config[SimParameter::Double::K_Off] = 0.0;
  config[SimParameter::Double::V_Pol] = 100.0;
  config[SimParameter::Double::V_Dep] = 0.0;
  config[SimParameter::Double::F_Cat] = 0.0;
  config[SimParameter::Double::F_Res] = 1.0;
  config[SimParameter::Double::T_End] = 150.0;
  config[SimParameter::Double::D_Rot] = 0.0;
  config[SimParameter::Double::D_Trans] = 0.0;
  config[SimParameter::Double::Spring_Length] = 0.1;
      
  return config;
}

// Reason: some inherited problem, need to be investigated
/*TEST(Force, Movement)
{
  try
  {
    double r_cell = SimParams::GetDefaultValue(SimParameter::Double::R_Cell, true);

    auto parameters = gcnew LaunchParameters();
    parameters->Config = ForceConfig();

    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, Math::PI / 2, 0.0, 0.0);
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, -1.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, -1.0, 0.0, 0.0, MTState::Polymerization);

    parameters->PoleCoords = gcnew PoleCoordinates();
    parameters->PoleCoords->AddRecord(0.0, -r_cell / 2, r_cell / 2, 0.0, r_cell / 2, r_cell / 2, 0.0);

    UNIFIED_TEST(parameters, ForceDistanceChecker, (Object ^)(r_cell / 1000));
  }
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }
  finally { Helper::ClearUpTestDirectory(); }
}*/

TEST(Force, Rotation)
{
  try
  {
    double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);

    auto parameters = gcnew LaunchParameters();
        
    parameters->Config = ForceConfig();
    parameters->Config[SimParameter::Int::Frozen_Coords] = 1;

    parameters->PoleCoords = gcnew PoleCoordinates();
    parameters->PoleCoords->AddRecord(0.0,  -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
    parameters->PoleCoords->AddRecord(10.0, -l_poles / 2, 0.0, 0.0, l_poles / 2, 0.0, 0.0);
    parameters->PoleCoords->AddRecord(50.0, 0.0, 0.0, -l_poles / 2, 0.0, 0.0, l_poles / 2);
    parameters->PoleCoords->AddRecord(90.0, 0.0, l_poles / 2, 0.0, 0.0,-l_poles / 2, 0.0);
    parameters->PoleCoords->AddRecord(130.0, 0.0, 0.0, l_poles / 2, 0.0, 0.0, -l_poles / 2);

    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddChromosomePair(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.0, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, -0.0, 0.0, 0.0, MTState::Polymerization);

    UNIFIED_TEST(parameters, ForceOrientationChecker, 0.01);
  }
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }
  finally { Helper::ClearUpTestDirectory(); }
}

TEST(Force, MovementAndRotation)
{
  try
  {
    double l_poles = SimParams::GetDefaultValue(SimParameter::Double::L_Poles, true);

    auto parameters = gcnew LaunchParameters();
    parameters->Config = ForceConfig();

    parameters->PoleCoords = gcnew PoleCoordinates();
    parameters->PoleCoords->AddRecord(0.0, -l_poles / 2, -l_poles / 4, -l_poles / 140 * 3, l_poles / 2, -l_poles / 4, -l_poles / 140 * 3);
    parameters->PoleCoords->AddRecord(4.1, -l_poles / 2, -l_poles / 4, -l_poles / 140 * 3, l_poles / 2, -l_poles / 4, -l_poles / 140 * 3);
    parameters->PoleCoords->AddRecord(4.2, l_poles / 2, -l_poles / 4, -l_poles / 140 * 3, -l_poles / 2, -l_poles / 4, -l_poles / 140 * 3);

    parameters->InitialStates = gcnew InitialStates();
    parameters->InitialStates->AddChromosomePair(0.0, -l_poles / 4, -l_poles / 140 * 3, 0.0, 0.0, 0.0);
    parameters->InitialStates->AddMT(PoleType::Left, 1.0, 0.0, 0.001, 0.0, MTState::Polymerization);
    parameters->InitialStates->AddMT(PoleType::Right, -1.0, 0.0, -0.001, 0.0, MTState::Polymerization);

    UNIFIED_TEST(parameters, ForceDistanceAndOrientationChecker, 0.01);
  }
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }
  finally { Helper::ClearUpTestDirectory(); }
}
