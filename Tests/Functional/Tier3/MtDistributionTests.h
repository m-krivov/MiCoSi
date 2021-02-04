#pragma once
#include "Defs.h"
#include "Helpers.h"


static inline String ^MTDistributionChecker(TimeStream ^ts, Object ^obj)
{
  double offset = 3;
  double size = 0.5;

  Dictionary<bool, int> ^hitX = gcnew Dictionary<bool, int>(),
                        ^hitY = gcnew Dictionary<bool, int>(),
                        ^hitZ = gcnew Dictionary<bool, int>();
  hitX[false] = 0; hitX[true] = 0;
  hitY[false] = 0; hitY[true] = 0;
  hitZ[false] = 0; hitZ[true] = 0;

  ts->MoveTo(ts->LayerCount - 1);
  auto cell = ts->Current->Cell;
  double r_cell  = ts->Current->SimParams->GetValue(SimParameter::Double::R_Cell, false);
  double l_poles = ts->Current->SimParams->GetValue(SimParameter::Double::L_Poles, false);

  for each (auto mt in cell->MTs)
  {
    double end_x = mt->Pole->Position.X + mt->Direction.X * mt->Length;
    double end_y = mt->Pole->Position.Y + mt->Direction.Y * mt->Length;
    double end_z = mt->Pole->Position.Z + mt->Direction.Z * mt->Length;
    if (Math::Abs(Math::Sqrt(end_x * end_x + end_y * end_y + end_z * end_z) - r_cell * 1e-6) > 0.1 * r_cell * 1e-6)
    { return gcnew String("At least one MT is too short"); }

    bool left = mt->Pole->Type == PoleType::Left;
    int sign = left ? -1 : 1;
    double dx = mt->Direction.X, dy = mt->Direction.Y, dz = mt->Direction.Z;

    // X-check
    if (Math::Abs(dx) > 1e-3)
    {
      double t = -sign * offset / dx;
      if (t >= 0)
      {
        double x = dx * t + sign * l_poles / 2;
        double y = dy * t;
        double z = dz * t;
        if (y * y + z * z <= size * size)
          hitX[left] += 1;
      }
    }

    // Y-check
    if (Math::Abs(dy) > 1e-3)
    {
      double t = offset / dy;
      if (t >= 0)
      {
        double x = dx * t + sign * l_poles / 2;
        double y = dy * t;
        double z = dz * t;
        if ((x - sign * l_poles / 2) * (x - sign * l_poles / 2) + z * z <= size * size)
          hitY[left] += 1;
      }
    }

    // Z-check
    if (Math::Abs(dz) > 1e-3)
    {
      double t = offset / dz;
      if (t >= 0)
      {
        double x = dx * t + sign * l_poles / 2;
        double y = dy * t;
        double z = dz * t;
        if ((x - sign * l_poles / 2) * (x - sign * l_poles / 2) + y * y <= size * size)
          hitZ[left] += 1;
      }
    }
  }

  auto poles = gcnew cli::array<bool> { false, true };
  for each (auto pole in poles)
  {
    if (hitX[pole] < 5 || hitY[pole] < 5 || hitZ[pole] < 5)
    { return gcnew String("Not enough hits"); }
    double av = (hitX[pole] / 2.0 + hitY[pole] + hitZ[pole]) / 3;
    if (Math::Abs(hitX[pole] / 2.0 - av) > 0.5 * av ||             // 50%
        Math::Abs(hitY[pole] - av) > 0.5 * av ||
        Math::Abs(hitZ[pole] - av) > 0.5 * av)
    { return gcnew String("Bad distribution #1"); }
  }

  double av2 = ((hitX[false] + hitX[true]) / 2.0 + (hitY[false] + hitY[true]) + (hitZ[false] + hitZ[true])) / 6;
  if (Math::Abs(hitX[false] - hitX[true]) / 2.0 > av2 ||
      Math::Abs(hitY[false] - hitY[true]) > av2 ||
      Math::Abs(hitZ[false] - hitZ[true]) > av2)
  {  return gcnew String("Bad distribution #2"); }

  return nullptr;
}

TEST(MtDistribution, InitialDirections)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Args = gcnew CliArgs();
  parameters->Args->UserSeed = 100500;

  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_MT_Total] = 4000;
  parameters->Config[SimParameter::Int::N_Cr_Total] = 0;
  parameters->Config[SimParameter::Double::L_Poles] = 10;
  parameters->Config[SimParameter::Double::R_Cell] = 6;
  parameters->Config[SimParameter::Double::V_Pol] = 200.0;
  parameters->Config[SimParameter::Double::V_Dep] = 0;
  parameters->Config[SimParameter::Double::F_Cat] = 0.0;
  parameters->Config[SimParameter::Double::T_End] = 20.0;

  UNIFIED_TEST(parameters, MTDistributionChecker, nullptr);
}

TEST(MtDistribution, NewDirections)
{
  auto parameters = gcnew LaunchParameters();
  parameters->Args = gcnew CliArgs();
  parameters->Args->UserSeed = 100500;

  parameters->Config = gcnew SimParams();
  parameters->Config[SimParameter::Int::N_MT_Total] = 4000;
  parameters->Config[SimParameter::Int::N_Cr_Total] = 0;
  parameters->Config[SimParameter::Double::L_Poles] = 10;
  parameters->Config[SimParameter::Double::R_Cell] = 6;
  parameters->Config[SimParameter::Double::V_Pol] = 200.0;
  parameters->Config[SimParameter::Double::V_Dep] = 0;
  parameters->Config[SimParameter::Double::F_Cat] = 0.0;
  parameters->Config[SimParameter::Double::T_End] = 20.0;

  parameters->InitialStates = gcnew InitialStates();
  for (int i = 0; i < parameters->Config[SimParameter::Int::N_MT_Total] * 2; i++)
  {
    bool left = i < parameters->Config[SimParameter::Int::N_MT_Total];
    parameters->InitialStates->AddMT(left ? PoleType::Left : PoleType::Right, 1.0, 0.0, 0.0, 0.0, MTState::Depolymerization);
  }

  UNIFIED_TEST(parameters, MTDistributionChecker, nullptr);
}
