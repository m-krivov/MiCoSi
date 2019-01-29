
#include "ExpSimulator.h"

#include "ttgUtils.h"
#include "Mitosis.Core/All.h"
#include "Mitosis.Objects/All.h"
#include "Mitosis.Geometry/Geometry.h"

//--------------------
//--- ExpSimulator ---
//--------------------

static inline double GetStandardNormal(uint32_t &seed, int n = 12)
{
  double sum = 0;
  for (int i = 0; i < n; i++)
    sum += (Random::NextReal(seed) - 0.5);
  if (n != 12)
    sum /= sqrt((double)n / 12);
  return sum;
}

void ExpSimulator::DoMacroStep(Cell *cell, uint32_t &seed)
{
  ttg_time_stamp("ExpSimulator::DoMacroStep()");
  real r_cell          = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::R_Cell, true);
  real dt              = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Dt, true);
  real A               = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Const_A, true);
  real b               = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Const_B, true);
  real Dtrans          = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::D_Trans, true);
  real Drot            = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::D_Rot, true);
  real gamma           = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Gamma, true);
  real ieta            = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Ieta, true);
  real cr_spring_l     = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Spring_Length, true);
  real cr_kin_r        = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_D, true) / 2;
  real cr_kin_l        = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_L, true);
  real v_pol           = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::V_Pol, true);
  real v_dep           = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::V_Dep, true);
  bool moving_spring   = GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::Spring_Type) == 1;
  bool move_non_broken = !GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::Frozen_Coords);
  bool mt_wrapping     = GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::MT_Wrapping);
  real cr_spring_k     = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Spring_K, true);
  const std::vector<Chromosome *> &chrs = cell->Chromosomes();
  for (int cri = 0; cri < chrs.size(); cri += (1 + !cell->AreSpringsBroken()))
  {
    Chromosome *crs[2];
    crs[0] = chrs[cri];
    crs[1] = chrs[cri ^ 1];
    vec3r V = vec3r::ZERO;
    vec3r w = vec3r::ZERO;
    real mat[6][7]; //Vx, Wx, Vy, Wy, Vz, Wz
    // Filling extended matrix
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 7; j++)
        mat[i][j] = 0;
    for (int i = 0; i < 3; i++)
    {
      mat[2 * i][2 * i] = -gamma;
      mat[2 * i + 1][2 * i + 1] = -ieta;
    }
    bool move_flag;
    if (cell->AreSpringsBroken())
      move_flag = !crs[0]->BoundMTs().empty();
    else
      move_flag = move_non_broken && (!crs[0]->BoundMTs().empty() || !crs[1]->BoundMTs().empty());
    for (int pairI = 0; pairI < 1 + !cell->AreSpringsBroken(); pairI++)
    {
      Chromosome *cr = crs[pairI];
      auto boundMTs = cr->BoundMTs();
      for (auto it = boundMTs.begin(); it != boundMTs.end(); it++)
      {
        MT *mt = *it;
        real len = mt->Length();

        // The following condition should be replaced by another condition - this one is wrong.
        // if (len > cr_kin_r * 2 && len > cr_kin_l * 2)
        {
          // Calculating and applying force point.
          if (mt_wrapping)
          {
            vec3r force_point;
            real refScale = r_cell;
            vec3r mt_end = mt->EndPoint();
            vec3r pole = mt->GetPole()->Position();
            
            vec3r chr_pos = (vec3r)cr->Position();
            mat3x3r orient = (mat3x3r)cr->Orientation();
            vec3r ortZ = (orient * vec3r((real)0.0, (real)1.0, (real)0.0)).Normalize();    // need to normalize due to
                                                    // single precision!
            
            // Projecting.
            vec3r mt_end_proj = mt_end + ortZ * DotProduct(ortZ, chr_pos - mt_end);
            vec3r pole_proj   = pole + ortZ * DotProduct(ortZ, chr_pos - pole);

            // Intersection check.
            if (Geometry::Distance(chr_pos, Geometry::Segment(mt_end_proj, pole_proj)) > cr_kin_r * (real)0.99)
            {
              force_point = mt->EndPoint();
            }
            else
            {
              vec3r p1, p2;

              // Getting vectors for new coordinate system.
              vec3r r1 = orient * vec3r((real)0.0, (real)0.0, -(real)cr_kin_r);
              vec3r r2 = orient * vec3r((real)cr_kin_r, (real)0.0, (real)0.0);

              // No tangent points, pole located inside sphere.
              if ((pole_proj - chr_pos).GetLength() < cr_kin_r * (real)1.01)
              {
                p1 = chr_pos + r1;
                p2 = chr_pos - r1;
              }
              // Have tangent points.
              else
              {
                // Computing tangent points.
                vec3r c = chr_pos - pole_proj;
                double cl = c.GetLength();
                double bl = cr_kin_r * cl / std::sqrt(cl * cl - cr_kin_r * cr_kin_r);
                vec3r b = CrossProduct(c.Normalize(), ortZ) * bl;
                vec3r nd1 = (c + b).Normalize(), nd2 = (c - b).Normalize();
                p1 = nd1 * DotProduct(nd1, c) + pole_proj;
                p2 = nd2 * DotProduct(nd2, c) + pole_proj;

                // Rounding tangent points.
                if (DotProduct(p1 - chr_pos, r2) < 0)
                  p1 = chr_pos + (DotProduct(pole_proj - chr_pos, r2) > 0 ? r1 : -r1);

                if (DotProduct(p2 - chr_pos, r2) < 0)
                  p2 = chr_pos + (DotProduct(pole_proj - chr_pos, r2) > 0 ? -r1 : r1);
              }

              // Selecting the closed variant.
              double dist1 = Geometry::ArcLength(chr_pos, cr_kin_r, mt_end_proj, p1);
              dist1 = std::min(dist1, (real)PI * cr_kin_r - dist1 + 2 * cr_kin_r);
              dist1 = (p1 - pole_proj).GetLength() + dist1;
              double dist2 = Geometry::ArcLength(chr_pos, cr_kin_r, mt_end_proj, p2);
              dist2 = std::min(dist2, (real)PI * cr_kin_r - dist2 + 2 * cr_kin_r);
              dist2 = (p2 - pole_proj).GetLength() + dist2;
              force_point = dist1 < dist2 ? p1 : p2;

              // Preparing multiplier for height detection.
              vec3r my_ort = (mt_end_proj - pole_proj);      // We will project force_point onto my_ort X ortZ plane
              real pole_mt_x = my_ort.GetLength();
              my_ort = my_ort / pole_mt_x;
              real y_mult = pole_mt_x;
              real height_eps = cr_kin_l * (real)1e-3;
              if (std::abs(y_mult) < height_eps ) y_mult = (y_mult >= 0 ? height_eps : -height_eps);
              y_mult = DotProduct(mt_end - pole, ortZ) / y_mult;

              // Constructing non-projected force point.
              real height = DotProduct(mt_end_proj - force_point, my_ort);      // first part, getting height without
                                                // scale multiplier and direction
              
              height *= pole_mt_x * height > 0.0 ? -y_mult : y_mult;        // adding multiplier with direction.
                                                // It depends on [mt_end, pole].x
                                                // and [mt_end, force_point].x segments

              height += DotProduct(mt_end - chr_pos, ortZ);            // shifting projected plane up/down
              
              height = std::min(std::max(height, - cr_kin_l / 2), cr_kin_l / 2);  // and finally rounding
              force_point += ortZ * height;
            }
            mt->ForceOffset() = force_point - mt->EndPoint();
          }
          else
            mt->ForceOffset() = vec3r::ZERO;

          // Updating matrix.
          vec3r beg = mt->GetPole()->Position();
          vec3r end = mt->ForcePoint();
          vec3r r = end - cr->Position();
          vec3r R = (beg - end).Normalize();
          vec3r rR = CrossProduct(r, R);

          for (int i = 0; i < 3; i++)
          {
            for (int j = 0; j < 3; j++)
            {
              mat[2 * i][2 * j] -= R[i] * R[j] * b;
              mat[2 * i][2 * j + 1] -= R[i] * rR[j] * b;
            }
            mat[2 * i][6] -= R[i] * A;
            for (int j = 0; j < 3; j++)
            {
              mat[2 * i + 1][2 * j] -= rR[i] * R[j] * b;
              mat[2 * i + 1][2 * j + 1] -= rR[i] * rR[j] * b;
            }
            mat[2 * i + 1][6] -= rR[i] * A;
          }
        }
      }
    }
    bool badFlag = false;
    // Normalizing matrix
    for (int i = 0; !badFlag && i < 6; i++)
    {
      real maxVal = 0;
      for (int j = 0; j < 6; j++)
        if (fabs(mat[i][j]) > maxVal)
          maxVal = fabs(mat[i][j]);
      if (maxVal > 0)
      {
        for (int j = 0; j < 7; j++)
          mat[i][j] /= maxVal;
      }
      else
      {
        badFlag = true;
      }
    }
    // Solving
    // Method of rotations
    // Move forward
    for (int line = 0; !badFlag && line < 5; line++)
    {
      int maxLine = line;
      real maxVal = fabs(mat[line][line]);
      for (int i = line + 1; i < 6; i++)
        if (fabs(mat[i][line]) > maxVal)
        {
          maxVal = fabs(mat[i][line]);
          maxLine = i;
        }
      if (maxVal > 0)
      {
        if (maxLine != line)
          for (int j = 0; j < 7; j++)
          {
            real tmp = mat[line][j];
            mat[line][j] = mat[maxLine][j];
            mat[maxLine][j] = tmp;
          }
        for (int i = line + 1; i < 6; i++)
        {
          real denom = sqrt(mat[line][line] * mat[line][line] + mat[i][line] * mat[i][line]);
          real c = mat[line][line] / denom;
          real s = mat[i][line] / denom;
          for (int j = line; j < 7; j++)
          {
            real fir = mat[line][j];
            real sec = mat[i][j];
            mat[line][j] = c * fir + s * sec;
            mat[i][j] = - s * fir + c * sec;
          }
        }
      }
      else
      {
        badFlag = true;
      }
    }
    // Move back
    for (int line = 5; !badFlag && line >= 0; line--)
    {
      if (fabs(mat[line][line]) > 0)
      {
        mat[line][6] /= mat[line][line];
        mat[line][line] = 1;
        for (int i = 0; i < line; i++)
        {
          mat[i][6] -= mat[line][6] * mat[i][line];
          mat[i][line] = 0;
        }
      }
      else
      {
        badFlag = true;
      }
    }
    // The same - wrong stabilizer, must to be replaced.
    // if (!badFlag)
    // {
    //   if (v_pol > 0 || v_dep > 0)
    //     if (vec3r(mat[0][6], mat[2][6], mat[4][6]).GetLength() > std::max(v_pol, v_dep) * 10)
    //       badFlag = true;
    //   if (vec3r(mat[1][6], mat[3][6], mat[5][6]).GetLength() > 1)
    //     badFlag = true;
    // }
    if (!badFlag)
    {
      // Now matrix is identity and the answer is in mat[*][6]
      w = vec3r(mat[1][6], mat[3][6], mat[5][6]);

      if (move_flag)
      {
        V = vec3r(mat[0][6], mat[2][6], mat[4][6]);
        if (!cell->AreSpringsBroken())
        {
          // Need to project velocity on the plane orthogonal to Pole-Pole line. And poles can be located in the same point!
          vec3r dir = (vec3r)cell->GetPole(PoleType::Left)->Position() - (vec3r)cell->GetPole(PoleType::Right)->Position();
          real dir_len = dir.GetLength();
          V = dir_len > r_cell * 1e-5 ? (vec3r)(V - dir * DotProduct(V, dir / dir_len) / dir_len) : vec3r::ZERO;

        }
      }
    }
    if (!cell->AreSpringsBroken())
    {
      // Need to update spring length
      vec3r center = ((vec3r)crs[0]->Position() + (vec3r)crs[1]->Position()) / (real)2.0;
      real prevLen = ((vec3r)crs[0]->Position() - (vec3r)crs[1]->Position()).GetLength();
      real newLen = cr_spring_l;
      if (moving_spring)
      {
        real totalForce = 0;
        for (int i = 0; i < 2; i++)
        {
          Chromosome *cr = crs[i];
          mat3x3r chrOrient = (mat3x3r)cr->Orientation();
          auto boundMTs = cr->BoundMTs();
          for (auto it = boundMTs.begin(); it != boundMTs.end(); it++)
          {
            MT *mt = *it;
            vec3r beg = mt->GetPole()->Position();
            vec3r end = beg + (vec3r)mt->Direction() * mt->Length();
            vec3r r = end - cr->Position();
            vec3r R = (beg - end).Normalize();
            vec3r crAxis = (chrOrient * vec3r(0.0, 1.0, 0.0)).Normalize();
            vec3r endPrj = (vec3r)cr->Position() + crAxis * DotProduct(r, crAxis);
            vec3r normAxis = (endPrj - end).Normalize();
            vec3r k = vec3r::ZERO;
            if (DotProduct(R, normAxis) <= 0)
              k = R;
            else
            {
              k = R - normAxis * DotProduct(R, normAxis);
              if (k.GetLength() > 1e-8)
                k = k.Normalize();
              else
                k = vec3r::ZERO;
            }
            vec3r springAxis = (chrOrient * vec3r(1.0, 0.0, 0.0)).Normalize();
            real ourForce = DotProduct(k * A, springAxis);
            totalForce += ourForce;
          }
        }
        if (totalForce < 0)
          newLen = 0;
        else
          newLen = cr_spring_l + (totalForce / 2) / cr_spring_k;
      }
      for (int i = 0; i < 2; i++)
      {
        Chromosome *cr = crs[i];
        vec3r springAxis = ((mat3x3r)cr->Orientation() * vec3r(1.0, 0.0, 0.0)).Normalize();
        cr->Position() = center + springAxis * (newLen / 2);
        auto boundMTs = cr->BoundMTs();
        for (auto it = boundMTs.begin(); it != boundMTs.end(); it++)
        {
          MT *mt = *it;
          vec3r beg = mt->GetPole()->Position();
          vec3r end = beg + (vec3r)mt->Direction() * mt->Length();
          end -= springAxis * (prevLen / 2);
          end += springAxis * (newLen / 2);
          mt->Direction() = (end - beg).Normalize();
          mt->Length() = (end - beg).GetLength();
        }
      }
    }
    // Langevin's members
    real rnd1 = (real)GetStandardNormal(seed);
    real rnd2 = (real)GetStandardNormal(seed);
    real rnd3 = (real)GetStandardNormal(seed);
    vec3r transAdd = vec3r(rnd1 * sqrt(2 * Dtrans * dt), rnd2 * sqrt(2 * Dtrans * dt), rnd3 * sqrt(2 * Dtrans * dt));
    rnd1 = (real)GetStandardNormal(seed);
    rnd2 = (real)GetStandardNormal(seed);
    rnd3 = (real)GetStandardNormal(seed);
    vec3r rotateAdd = vec3r(real(rnd1 * sqrt(2 * Drot * dt)), rnd2 * sqrt(2 * Drot * dt), rnd3 * sqrt(2 * Drot * dt));
    
    // Applying velocities and Langevin's members
    vec3r trans = V * dt + transAdd;
    mat3x3r rotate = MatrixRotationXYZ<real>(w[0] * dt + rotateAdd[0], w[1] * dt + rotateAdd[1], w[2] * dt + rotateAdd[2]);

    //Updating chromosome states.
    vec3r rotatePoint = ((vec3r)crs[0]->Position() + (vec3r)crs[1]->Position()) / 2;
    for (int i = 0; i < 1 + !cell->AreSpringsBroken(); i++)
    {
      Chromosome *cr = crs[i];
      Chromosome *pairedCr = crs[1 - i];

      // Setting chromosome's position and orientation.
      vec3r prevPos = (vec3r)cr->Position();
      if (cell->AreSpringsBroken())
      {
        rotatePoint = (vec3r)cr->Position();
      }

      cr->Position() = rotatePoint + (rotate * (prevPos - rotatePoint) + trans);
      mat3x3r orient = rotate * cr->Orientation();
      cr->Orientation() = orient;

      // Setting bound MTs.
      vec3r ort = (orient * vec3r((real)0.0, (real)1.0, (real)0.0)).Normalize();
      vec3r springOffset = (vec3r)cr->Position() - (rotatePoint + trans);
      auto boundMTs = cr->BoundMTs();
      for (auto it = boundMTs.begin(); it != boundMTs.end(); it++)
      {
        MT *mt = *it;

        // Getting direction from chromosome's center to MT's end, updating it.
        vec3r beg = mt->GetPole()->Position();
        vec3r end = beg + (vec3r)mt->Direction() * mt->Length();
        vec3r r = end - rotatePoint;
        r = rotate * r;
        
        // Fighting with float-based errors.
        real height = DotProduct(r, ort);
        r = (r - springOffset - ort * height).Normalize() * cr_kin_r;
        r = r + springOffset + ort * std::max(-cr_kin_l / 2, std::min(cr_kin_l / 2, height));

        // Applying new MT's end point. Force point is the same due to small dt values and future recalculating.
        end = rotatePoint + r + trans;
        mt->Direction() = (end - beg).Normalize();
        mt->Length() = (end - beg).GetLength();
      }
    }
  }
  ttg_time_stamp("ExpSimulator::DoMacroStep()");
}

void ExpSimulator::DoMicroStep(Cell *cell, uint32_t &seed)
{
  ttg_time_stamp("ExpSimulator::DoMicroStep()");
  real dt          = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Dt, true);
  real radius      = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::R_Cell, true);
  real v_pol       = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::V_Pol, true);
  real v_dep       = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::V_Dep, true);
  real f_cat       = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::F_Cat, true);
  real f_res       = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::F_Res, true);
  real cr_hand_r   = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Hand_D, true) / 2;
  real cr_kin_r    = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_D, true) / 2;
  real cr_kin_cosa = (real)std::cos(GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_Angle, true) / 2);
  real cr_l        = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_L, true);
  real cr_kin_l    = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Cr_Kin_L, true);
  real k_on        = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::K_On, true);
  real k_off       = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::K_Off, true);
  real cr_hand_l   = (cr_l - cr_kin_l) / 2;
  int wi           = -1;
  const std::vector<MT *> &mts = cell->MTs();
  for (int i = 0; i < mts.size(); i++)
  {
    MT *mt = mts[i];
    if (mt->BoundChromosome() == NULL)
    {
      if (mt->State() == MTState::Polymerization)
      {
        mt->Length() += v_pol * dt;
        if (Random::NextReal(seed) < f_cat * dt)
        {
          mt->State() = MTState::Depolymerization;
        }
      }
      else
      {
        mt->Length() = std::max((real)0, mt->Length() - v_dep * dt);
        if (mt->Length() == (real)0 || Random::NextReal(seed) < f_res * dt)
        {
          mt->State() = MTState::Polymerization;
          if (mt->Length() == (real)0)
          {
            double alpha = Random::NextReal(seed) * PI * 2;
            real dx = Random::NextReal(seed) * (mt->GetPole()->Type() == PoleType::Left ? 1 : -1);
            real dy = (real)(std::sqrt(1.0 - dx * dx) * std::cos(alpha));
            real dz = (real)(std::sqrt(1.0 - dx * dx) * std::sin(alpha));
            vec3r dir(dx, dy, dz);
            mt->Direction() = dir;
          }
        }
      }
      vec3r beg = mt->GetPole()->Position();
      vec3r dir = (vec3r)mt->Direction();
      real len = mt->Length();
      vec3r end = beg + dir * len;
      if (mt->State() == MTState::Polymerization)
      {
        // Check cell boundaries
        if (end.GetLength() >= radius)
        {
          mt->State() = MTState::Depolymerization;
        }
      }
      vec3r interPoint;
      const std::vector<Chromosome *> &chrs = cell->Chromosomes();
      if(mt->State() == MTState::Polymerization)
      {
        // Intersect with hands or plain side
        bool intersects = false;
        bool pls = false;
        for (int j = 0; j < chrs.size(); j++)
        {
          Chromosome *cr = chrs[j];
          mat3x3r chrOrient = (mat3x3r)cr->Orientation();
          vec3r ortX = (chrOrient * vec3r(1, 0, 0)).Normalize();
          vec3r ortY = (chrOrient * vec3r(0, 1, 0)).Normalize();
          vec3r ortZ = (chrOrient * vec3r(0, 0, 1)).Normalize();
          vec3r plR1 = ortZ * -cr_hand_r;
          vec3r plR2 = ortX *  cr_hand_r;
          // Hands
          // Upper hand
          Geometry::Segment seg(beg, end);
          vec3r handBeg = (vec3r)cr->Position() + ortY * (real)(cr_kin_l / 2);
          vec3r handEnd = (vec3r)cr->Position() + ortY * (real)(cr_l / 2);

          if (Geometry::AreIntersected(seg, Geometry::SemiTube(handBeg, handEnd, plR2), interPoint) ||
            Geometry::AreIntersected(seg, Geometry::Rectangle(handBeg - plR1, plR1 * 2, handEnd - handBeg), interPoint) ||
            Geometry::AreIntersected(seg, Geometry::SemiCircle(handBeg, plR1, plR2), interPoint) ||
            Geometry::AreIntersected(seg, Geometry::SemiCircle(handEnd, plR1, plR2), interPoint))
          {
            intersects = true;
            break;
          }

          // Lower hand
          handBeg = (vec3r)cr->Position() + ortY * (real)(-cr_kin_l / 2);
          handEnd = (vec3r)cr->Position() + ortY * (real)(-cr_l / 2);
          if (Geometry::AreIntersected(seg, Geometry::SemiTube(handBeg, handEnd, plR2), interPoint) ||
            Geometry::AreIntersected(seg, Geometry::Rectangle(handBeg - plR1, plR1 * 2, handEnd - handBeg), interPoint) ||
            Geometry::AreIntersected(seg, Geometry::SemiCircle(handBeg, plR1, plR2), interPoint) ||
            Geometry::AreIntersected(seg, Geometry::SemiCircle(handEnd, plR1, plR2), interPoint))
          {
            intersects = true;
            break;
          }

          // Kinetchore back plane.
          if (Geometry::AreIntersected(seg,
                         Geometry::Rectangle(handBeg - ortZ * cr_kin_r,
                                   ortZ * (2 * cr_kin_r),
                                   ortY * cr_kin_l),
                         interPoint))
          {
            intersects = true;
            break;
          }
        }
        if (intersects)
        {
          mt->State() = MTState::Depolymerization;
        }
      }
      // Intersect with kinetochore (e.g. semi-cylinder)
      int minKinIdx = -1;
      real minKinLen = mt->Length();
      for (int j = 0; j < chrs.size(); j++)
      {
        Chromosome *cr = chrs[j];
        mat3x3r chrOrient = (mat3x3r)cr->Orientation();
        vec3r ortX = (chrOrient * vec3r(1, 0, 0)).Normalize();
        vec3r ortY = (chrOrient * vec3r(0, 1, 0)).Normalize();
        vec3r plNorm = ortX * cr_kin_r;
        vec3r kinBeg = (vec3r)cr->Position() + chrOrient * vec3r((real)0, (real)(-cr_kin_l / 2), (real)0);
        vec3r kinEnd = (vec3r)cr->Position() + chrOrient * vec3r((real)0, (real)(cr_kin_l / 2), (real)0);
        if (Geometry::AreIntersected(Geometry::Segment(beg, end),
                       Geometry::SemiTube(kinBeg, kinEnd, plNorm),
                       interPoint))
        {
          // Collided, but we need to check the angle as well.
          vec3r dp = interPoint - kinBeg;
          dp = dp - ortY * DotProduct(dp, ortY);
          
          if (DotProduct(dp, ortX) >= cr_kin_cosa * cr_kin_r &&
            (interPoint - beg).GetLength() <= minKinLen)
          {
            // Ok, MT can be attached.
            minKinIdx = j;
            minKinLen = (interPoint-beg).GetLength();
          }
          mt->State() = MTState::Depolymerization;
        }
      }
      if(minKinIdx != -1)
      {
        if (Random::NextReal(seed) < k_on * dt)
        {
          mt->Bind(chrs[minKinIdx]);
        }
        mt->Length() = minKinLen;
      }
    }
    else
    {
      if (Random::NextReal(seed) < k_off * dt)
      {
        // Detach MT from chromosome
        Chromosome *cr = mt->BoundChromosome();
        mt->UnBind();
        mt->State() = MTState::Depolymerization;
      }
    }
  }
  ttg_time_stamp("ExpSimulator::DoMicroStep()");
}

void ExpSimulator::DoPoleUpdatingStep(Cell *cell, uint32_t &seed, IPoleUpdater *updater, double time)
{
  ttg_time_stamp("ExpSimulator::DoPoleUpdatingStep()");
  real dt        = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Dt);
  vec3r oldLeft  = (vec3r)cell->GetPole(PoleType::Left)->Position();
  vec3r oldRight = (vec3r)cell->GetPole(PoleType::Right)->Position();

  updater->MovePoles(cell->GetPole(PoleType::Left), cell->GetPole(PoleType::Right), (real)time + dt, seed);
  if (oldLeft != cell->GetPole(PoleType::Left)->Position() ||
    oldRight != cell->GetPole(PoleType::Right)->Position())
  {
    vec3r leftPoleOffset = (vec3r)cell->GetPole(PoleType::Left)->Position() - oldLeft;
    vec3r rightPoleOffset = (vec3r)cell->GetPole(PoleType::Right)->Position() - oldRight;
    const std::vector<MT *> &mts = cell->MTs();
    for (size_t i = 0; i < mts.size(); i++)
    {
      if (mts[i]->BoundChromosome() != NULL)
      {
        vec3r oldVec = (vec3r)mts[i]->Direction() * mts[i]->Length();
        vec3r newVec = oldVec - (mts[i]->GetPole()->Type() == PoleType::Left
                ? leftPoleOffset
                : rightPoleOffset);
        mts[i]->Length() = newVec.GetLength();
        mts[i]->Direction() = mts[i]->Length() == 0.0 ? vec3r::DEFAULT_DIRECT : (vec3r)newVec.Normalize();
      }
    }
  }
  ttg_time_stamp("ExpSimulator::DoPoleUpdatingStep()");
}

void ExpSimulator::DoSpringBreakingStep(Cell *cell, uint32_t &seed)
{
  ttg_time_stamp("ExpSimulator::DoSpringBreakingStep()");
  if(cell->Chromosomes().size() == 0)
  {
    ttg_time_stamp("ExpSimulator::DoSpringBreakingStep()");
    return;
  }
  if (!cell->AreSpringsBroken())
  {
    if (GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::Spring_Brake_Type) == 1)
    {
      int minCount = GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::Spring_Brake_MTs) * 2;
      const std::vector<Chromosome *> &chrs = cell->Chromosomes();
      for (size_t i = 0; i < chrs.size(); i++)
      {
        Chromosome *cr = chrs[i];
        if (minCount > cr->BoundMTs().size())
          minCount = (int)cr->BoundMTs().size();
      }
      if (minCount >= GlobalSimParams::GetRef()->GetParameter(SimParameter::Int::Spring_Brake_MTs))
        cell->SetSpringFlag(true);
    }
    else
    {
      real minForce = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Spring_Brake_Force, true) * 2;
      real const_a  = (real)GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Const_A, true);
      const std::vector<Chromosome *> &chrs = cell->Chromosomes();
      for (size_t i = 0; i < chrs.size(); i++)
      {
        Chromosome *cr = chrs[i];
        vec3r curForce(0, 0, 0);
        auto boundMTs = cr->BoundMTs();
        for (auto it = boundMTs.begin(); it != boundMTs.end(); it++)
        {
          MT *mt = *it;
          curForce = curForce + ((mt->GetPole()->Position() - cr->Position()).Normalize() * const_a);
        }
        real curForceMod = curForce.GetLength();
        if (minForce > curForceMod)
          minForce = curForceMod;
      }
      if (minForce >= GlobalSimParams::GetRef()->GetParameter(SimParameter::Double::Spring_Brake_Force, true))
        cell->SetSpringFlag(true);
    }
  }
  ttg_time_stamp("ExpSimulator::DoSpringBreakingStep()");
}
