#include "RandomCellInitializer.h"

#include "MiCoSi.Core/All.h"
#include "MiCoSi.Objects/All.h"
#include "MiCoSi.Geometry/Geometry.h"

//-----------------------------
//--- RandomCellInitialzier ---
//-----------------------------

void RandomCellInitialzier::GetCellConfig(size_t &chrPairs, size_t &mtsPerPole)
{
  chrPairs  = (size_t)GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::N_Cr_Total);
  mtsPerPole  = (size_t)GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::N_MT_Total);
}

void RandomCellInitialzier::InitializeCell(ICell *cell, Random::State &state)
{
  double r_cell = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::R_Cell, true);
  double l_poles = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::L_Poles, true);
  double cr_l = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_L, true);
  double cr_kin_l = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_L, true);
  double cr_hand_r = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Hand_D, true) / 2;
  double cr_kin_r = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_D, true) / 2;
  double cr_spring_l = GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Spring_Length, true);
  Geometry geom((real)(r_cell * 1e-5));

  // Set spring flag
  cell->SetSpringFlag(false);

  // Initialize poles
  vec3r pos = vec3r::DEFAULT_LEFT * (l_poles / 2);
  cell->GetPole(PoleType::Left)->Position() = pos;
  cell->GetPole(PoleType::Right)->Position() = -pos;

  // Initialize micro tubules
  const std::vector<MT *> &MTs = cell->MTs();
  for (size_t i = 0; i < MTs.size(); i++)
  {
    double alpha = Random::NextReal(state) * PI * 2;
    real dx = Random::NextReal(state) * (MTs[i]->GetPole()->Type() == PoleType::Left ? 1 : -1);
    real dy = (real)(std::sqrt(1.0 - dx * dx) * std::cos(alpha));
    real dz = (real)(std::sqrt(1.0 - dx * dx) * std::sin(alpha));
    MTs[i]->Direction() = vec3r(dx, dy, dz);
    MTs[i]->Length() = (real)0.0;
    MTs[i]->State() = MTState::Polymerization;
  }

  // Inititalize chromosomes
  // Pair of Chromosomes is (half-sphere + cylinder) + cylinder + (cylinder + half-sphere)
  const std::vector<Chromosome *> &chrs = cell->Chromosomes();
  double cr_r = std::max(cr_hand_r, cr_kin_r);
  double cr_hand_l = (cr_l - cr_kin_l) / 2;
  real cellEdgeGap = 0;
  real betweenGap = 0;
  for (size_t i = 0; i < chrs.size() / 2; i++)
  {
    // Position of center of the pair of chromosomes
    vec3r pos;
    pos.x = (cell->GetPole(PoleType::Left)->Position().x + cell->GetPole(PoleType::Right)->Position().x) / 2;
    // rotation matrix around pos (firstly apply matrix, then apply offset)
    // initial orientation is: center of mass in (0,0,0), kinet and hands in (+radius, +-length, +-radius)
    mat3x3r orient;
    bool canFlag;
    do
    {
      real xAngle = (real)(Random::NextReal(state) * 2 * PI);
      orient = MatrixRotationX(xAngle);
      pos.y = (real)(Random::NextReal(state) * (2 * r_cell) - r_cell);
      pos.z = (real)(Random::NextReal(state) * (2 * r_cell) - r_cell);
      // Check if we can place pair of chromosomes here
      canFlag = true;
      // Check cell boundaries
      vec3r topEdge = pos + orient * vec3r::DEFAULT_UP * cr_l / 2;
      vec3r bottomEdge = pos + orient * (vec3r::DEFAULT_UP * (-1)) * cr_l / 2;
      if (topEdge.GetLength() + cr_hand_r + cr_spring_l / 2 + cellEdgeGap >= r_cell)
        canFlag = false;
      if (bottomEdge.GetLength() + cr_hand_r + cr_spring_l / 2 + cellEdgeGap >= r_cell)
        canFlag = false;
      if (!canFlag)
        continue;
      // Check intersection with other chromosomes
      for (size_t j = 0; canFlag && j < i; j++)
      {
        vec3r pos2 = (vec3r)chrs[2 * j]->Position();
        mat3x3r orient2 = (mat3x3r)chrs[2 * j]->Orientation();
        vec3r topEdge2 = pos2 + orient2 * vec3r::DEFAULT_UP * cr_l / 2;
        vec3r bottomEdge2 = pos2 + orient2 * (vec3r::DEFAULT_UP * (-1)) * cr_l / 2;
        // Check intersection
        vec3r inter;
        inter.x = topEdge.x;
        vec3r dir1 = (bottomEdge - topEdge).Normalize();
        vec3r dir2 = (bottomEdge2 - topEdge2).Normalize();
        double a1, b1, c1, a2, b2, c2;
        a1 = -dir1.z;
        b1 = dir1.y;
        a2 = -dir2.z;
        b2 = dir2.y;
        c1 = a1 * topEdge.y + b1 * topEdge.z;
        c2 = a2 * topEdge2.y + b2 * topEdge2.z;
        double det = a1 * b2 - a2 * b1;
        if (fabs(det) > 1e-8)
        {
          inter.y = (real)((c1 * b2 - c2 * b1) / det);
          inter.z = (real)((a1 * c2 - a2 * c1) / det);
          bool interFlag = fabs(((inter - topEdge).GetLength() + (inter - bottomEdge).GetLength() - cr_l) / cr_l) < 1e-4;
          interFlag = interFlag && fabs(((inter - topEdge2).GetLength() + (inter - bottomEdge2).GetLength() - cr_l) / cr_l) < 1e-4;
          if (interFlag)
            canFlag = false;
        }
        if (canFlag)
        {
          // Check distance
          // Distance between segments
          if (geom.Distance(Geometry::Segment(bottomEdge, topEdge),
                            Geometry::Segment(bottomEdge2, topEdge2)) < 2 * cr_r + cr_spring_l + betweenGap)
            canFlag = false;
        }
      }
    }
    while (!canFlag);
    mat3x3r secondOrient = orient * MatrixRotationY<real>((real)PI);
    chrs[2 * i]->Position() = pos + orient * vec3r(real(cr_spring_l / 2), real(0), real(0));
    chrs[2 * i]->Orientation() = orient;
    chrs[2 * i + 1]->Position() = pos + secondOrient * vec3r(real(cr_spring_l / 2), real(0), real(0));
    chrs[2 * i + 1]->Orientation() = secondOrient;
  }
}
