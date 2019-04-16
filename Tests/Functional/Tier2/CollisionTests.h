#pragma once
#include "Defs.h"
#include "Helpers.h""

using System::Drawing::Bitmap;
using System::Drawing::Color;
using System::Drawing::Graphics;

public ref class CollisionRender : public IDisposable
{
  private:
    double _dl, _da;
    cli::array<double, 2> ^_depthBuffer;
    Vec3 ^_screenNormal;
    double _coeffD;

    void ComputeCoeffs()
    {
      _screenNormal = Vec3::CrossProduct(ScreenUpVector, ScreenRightVector);
      Vec3 ^tmp = _screenNormal * ScreenAnchorPoint;
      _coeffD = -(tmp->x + tmp->y + tmp->z);
    }
    
  public:
    property Bitmap ^RenderTarget;
    property Vec3 ^ScreenAnchorPoint;
    property Vec3 ^ScreenUpVector;
    property Vec3 ^ScreenRightVector;
    property Vec3 ^ViewPoint;

    CollisionRender(double dLength, double dAngle, int rtSize)
    {
      _dl = dLength;
      _da = dAngle;
      RenderTarget = gcnew Bitmap(rtSize, rtSize);
      _depthBuffer = gcnew cli::array<double, 2>(rtSize, rtSize);
      for (int x = 0; x < rtSize; x++)
      {
        for (int y = 0; y < rtSize; y++)
        { _depthBuffer[x, y] = Double::MaxValue; }
      }

      ViewPoint = gcnew Vec3(-1.0, 0.0, 0.0);
      ScreenAnchorPoint = gcnew Vec3(-0.25, 0.5, -0.5);
      ScreenUpVector = gcnew Vec3(0.0, -1.0, 0.0);
      ScreenRightVector = gcnew Vec3(0.0, 0.0, 1.0);
    }

    void Clear(Color color)
    {
      Graphics ^gr = Graphics::FromImage(RenderTarget);
      gr->Clear(color);
      delete gr;
    }

    Tuple<int, int> ^Project(Vec3 ^point)
    {
      Vec3 ^p1 = ViewPoint;
      Vec3 ^dir = point - p1;

      Vec3 ^tmp1 = _screenNormal * p1;
      Vec3 ^tmp2 = _screenNormal * dir;
      double t = (-_coeffD - tmp1->x - tmp1->y - tmp1->z) / (tmp2->x + tmp2->y + tmp2->z);
      Vec3 ^projPoint = p1 + dir * t;

      int x = (int)Math::Round(Vec3::DotProduct(projPoint - ScreenAnchorPoint, ScreenRightVector->Normailzed)
                  / ScreenRightVector->Length * RenderTarget->Width, 0);
      int y = (int)Math::Round(Vec3::DotProduct(projPoint - ScreenAnchorPoint, ScreenUpVector->Normailzed)
                  / ScreenUpVector->Length * RenderTarget->Height);
      
      return gcnew Tuple<int, int>(x, y);
    }

    void ProjectAndDraw(Vec3 ^point, Color color)
    {

      auto pp = Project(point);
      int x = pp->Item1;
      int y = pp->Item2;
      double depth = (point - ViewPoint)->Length;
      if (0 <= x && x < RenderTarget->Width &&
          0 <= y && y < RenderTarget->Height &&
          _depthBuffer[x, y] > depth)
      {
        RenderTarget->SetPixel(x, y, color);
        _depthBuffer[x, y] = depth;
      }
    }

    void FillRectangle(Vec3 ^point, Vec3 ^v1, Vec3 ^v2, Color color)
    {
      ComputeCoeffs();

      double v1Len = v1->Length, v2Len = v2->Length;
      Vec3 ^nv1 = v1->Normailzed, ^nv2 = v2->Normailzed;
      double sv1 = 0.0, sv2 = 0.0;
      while (sv2 < v2Len)
      {
        while (sv1 < v1Len)
        {
          ProjectAndDraw(point + nv1 * sv1 + nv2 * sv2, color);
          sv1 += _dl;
        }
        sv1 = 0.0;
        sv2 += _dl;
      }
    }

    void FillArc(Vec3 ^centerPoint, Vec3 ^fromVec, Vec3 ^toVec, Color color)
    {
      ComputeCoeffs();

      double cl = 0.0, ca = 0.0;
      double lmax = fromVec->Length;
      double amax = Math::Acos(Vec3::DotProduct(fromVec->Normailzed, toVec->Normailzed));
      Vec3 ^ortVec = gcnew Vec3(0.0, 0.0, 0.0) -
                     Vec3::CrossProduct(fromVec, Vec3::CrossProduct(fromVec, toVec))->Normailzed;
      Vec3 ^nfromVec = fromVec->Normailzed;
      while (cl < lmax)
      {
        while (ca < amax)
        {
          ProjectAndDraw(centerPoint + (nfromVec * Math::Cos(ca) + ortVec * Math::Sin(ca)) * cl, color);
          ca += _da;
        }
        ca = 0.0;
        cl += _dl;
      }
    }

    void FillSemiCylinder(Vec3 ^centerPoint, Vec3 ^fromVec, Vec3 ^toVec, Vec3 ^axis, Color color)
    {
      ComputeCoeffs();

      double cl = 0.0, ca = 0.0;
      double lmax = axis->Length;
      double amax = Math::Acos(Vec3::DotProduct(fromVec->Normailzed, toVec->Normailzed));
      Vec3 ^ortVec = gcnew Vec3(0.0, 0.0, 0.0) -
                     Vec3::CrossProduct(fromVec, Vec3::CrossProduct(fromVec, toVec))->Normailzed * fromVec->Length;
      Vec3 ^naxis = axis->Normailzed;
      while (cl < lmax)
      {
        while (ca < amax)
        {
          ProjectAndDraw(centerPoint + fromVec * Math::Cos(ca) + ortVec * Math::Sin(ca) + naxis * cl, color);
          ca += _da;
        }
        ca = 0.0;
        cl += _dl;
      }
    }

    ~CollisionRender()
    {
      if (RenderTarget != nullptr)
      {
        delete RenderTarget;
        RenderTarget = nullptr;
      }
    }

    !CollisionRender()
    {
      if (RenderTarget != nullptr)
      {
        delete RenderTarget;
        RenderTarget = nullptr;
      }
    }
};

#define COLLISION_BACK_COLOR Color::Black
#define COLLISION_HAND_COLOR Color::Blue
#define COLLISION_KIN_COLOR  Color::Green

static inline void CollisionViewPoint(CollisionRender ^render, SimParams ^config)
{
  double l_poles = config[SimParameter::Double::L_Poles] * 1e-6;
  render->ViewPoint = gcnew Vec3(-l_poles / 2, 0.0, 0.0);
  render->ScreenAnchorPoint = gcnew Vec3(-l_poles / 4, l_poles / 16, -l_poles / 16);
  render->ScreenUpVector = gcnew Vec3(0.0, -l_poles / 8, 0.0);
  render->ScreenRightVector = gcnew Vec3(0.0, 0.0, l_poles / 8);
}

static inline void CollisionMagicQuaterCylinder(CollisionRender ^render,
                                                Vec3 ^rootPoint, Vec3 ^axisVec,
                                                Vec3 ^fromVec, Vec3 ^toVec,
                                                Color plainColor, Color curveColor)
{
  render->FillRectangle(rootPoint, fromVec, axisVec, plainColor);
  render->FillRectangle(rootPoint, toVec, axisVec, plainColor);
  render->FillArc(rootPoint, fromVec, toVec, curveColor);
  render->FillArc(rootPoint + axisVec, fromVec, toVec, curveColor);
  render->FillSemiCylinder(rootPoint, fromVec, toVec, axisVec, curveColor);
}

static inline void CollisionTwoMagicHalfCylinders(CollisionRender ^render,
                                                  Vec3 ^centerPoint, double length, double offset, double d,
                                                  Matrix3x3 ^orient,
                                                  Color plainColor, Color curveColor)
{
  auto toUp = orient->Rotate(gcnew Vec3(0.0, length / 2, 0.0));
  auto toLeft = orient->Rotate(gcnew Vec3(-offset / 2, 0.0, 0.0));

  CollisionMagicQuaterCylinder(render, centerPoint + toLeft + toUp,
                               toUp * (-2.0),
                               orient->Rotate(gcnew Vec3(0.0, 0.0, -d / 2.0)),
                               orient->Rotate(gcnew Vec3(-d / 2.0, 0.0, 0.0)),
                               plainColor, curveColor);
  CollisionMagicQuaterCylinder(render, centerPoint + toLeft + toUp,
                               toUp * (-2.0),
                               orient->Rotate(gcnew Vec3(-d / 2.0, 0.0, 0.0)),
                               orient->Rotate(gcnew Vec3(0.0, 0.0, d / 2.0)),
                               plainColor, curveColor);

  CollisionMagicQuaterCylinder(render, centerPoint - toLeft + toUp,
                               toUp * (-2.0),
                               orient->Rotate(gcnew Vec3(0.0, 0.0, -d / 2.0)),
                               orient->Rotate(gcnew Vec3(d / 2.0, 0.0, 0.0)),
                               plainColor, curveColor);
  CollisionMagicQuaterCylinder(render, centerPoint - toLeft + toUp,
                               toUp * (-2.0),
                               orient->Rotate(gcnew Vec3(d / 2.0, 0.0, 0.0)),
                               orient->Rotate(gcnew Vec3(0.0, 0.0, d / 2.0)),
                               plainColor, curveColor);
}

static inline void CollisionChromosome(CollisionRender ^render, LaunchParameters ^parameters,
                                       double position_x, double position_y, double position_z,
                                       double orientation_x, double orientation_y, double orientation_z)
{
  // Add chromosome to launch parameters
  if (parameters->InitialStates == nullptr)
  { parameters->InitialStates = gcnew InitialStates(); }
  parameters->InitialStates->AddChromosomePair(position_x, position_y, position_z,
                                               orientation_x, orientation_y, orientation_z);

  // Draw using render
  auto orient = Matrix3x3::RotationXYZ(orientation_x, orientation_y, orientation_z);
  auto position = gcnew Vec3(position_x, position_y, position_z);
  double cr_l = parameters->Config[SimParameter::Double::Cr_L] * 1e-6;
  double cr_kin_l = parameters->Config[SimParameter::Double::Cr_Kin_L] * 1e-6;
  double cr_kin_d = parameters->Config[SimParameter::Double::Cr_Kin_D] * 1e-6;
  double cr_hand_l = (cr_l - cr_kin_l) / 2.0;
  double cr_hand_d = parameters->Config[SimParameter::Double::Cr_Hand_D] * 1e-6;
  double spring_length = parameters->Config[SimParameter::Double::Spring_Length] * 1e-6;

  CollisionTwoMagicHalfCylinders(render, position + orient->Rotate(gcnew Vec3(0.0, (cr_hand_l + cr_kin_l) / 2.0, 0.0)),
                                 cr_hand_l, spring_length, cr_hand_d, orient,
                                 COLLISION_HAND_COLOR, COLLISION_HAND_COLOR);
  CollisionTwoMagicHalfCylinders(render, position + orient->Rotate(gcnew Vec3(0.0, -(cr_hand_l + cr_kin_l) / 2.0, 0.0)),
                                 cr_hand_l, spring_length, cr_hand_d, orient,
                                 COLLISION_HAND_COLOR, COLLISION_HAND_COLOR);
  CollisionTwoMagicHalfCylinders(render, position, cr_kin_l, spring_length, cr_kin_d, orient,
                                 COLLISION_HAND_COLOR, COLLISION_KIN_COLOR);
}

static inline  String ^CollisionTest(SimParams ^config,
                                     void (*initializer)(int mtN, int mtsPerPole,
                                                         [Out] double %dirX,
                                                         [Out] double %dirY,
                                                         [Out] double %dirZ),
                                     double px, double py, double pz,
                                     double ox, double oy, double oz)
{
  int mtsPerPole = config[SimParameter::Int::N_MT_Total];
  double r_cell = config[SimParameter::Double::R_Cell] * 1e-6;

  try
  {
    Helper::PrepareTestDirectory();
    auto parameters = gcnew LaunchParameters();
    parameters->Config = config;
    parameters->Args->RngSeed = 100500;

    CollisionRender ^render = nullptr;
    try
    {
      render = gcnew CollisionRender(r_cell / 2500.0, Math::PI / 360, 1000);

      // Basic settings
      CollisionViewPoint(render, parameters->Config);
      render->Clear(COLLISION_BACK_COLOR);
      CollisionChromosome(render, parameters, px, py, pz, ox, oy, oz);

      //Setting MTs.
      auto lMTs = gcnew cli::array<Tuple<int, int> ^>(mtsPerPole);
      for (int i = 0; i < mtsPerPole; i++)
      {
        double dirX = 0.0, dirY = 0.0, dirZ = 0.0;

        initializer(i, mtsPerPole, dirX, dirY, dirZ);
        parameters->InitialStates->AddMT(PoleType::Left, dirX, dirY, dirZ, 0.0, MTState::Polymerization);
        parameters->InitialStates->AddMT(PoleType::Right, 0.0, 1.0, 0.0, 0.0, MTState::Polymerization);
        lMTs[i] = render->Project(gcnew Vec3(dirX, dirY, dirZ));
      }

      //Launching simulation, checking results.
      auto rt = render->RenderTarget;
      TimeStream ^ts = nullptr;
      Bitmap ^debugRT = nullptr;
      try
      {
        ts = Helper::LaunchAndOpen(parameters);
        debugRT = gcnew Bitmap(render->RenderTarget);

        ts->MoveTo(ts->LayerCount - 1);
        auto cell = ts->Current->Cell;
        int n = 0;
        int kinMTs = 0, handMTs = 0, cellMTs = 0;
        for each (auto mt in cell->MTs)
        {
          if (mt->Pole->Type == PoleType::Left)
          {
            int x = lMTs[n]->Item1, y = lMTs[n]->Item2;
            if (0 <= x && x < rt->Width && 0 <= y && y < rt->Height)
            {
              auto mtEnd = gcnew Vec3(cell->GetPole(PoleType::Left)->Position) +
                           gcnew Vec3(mt->Direction) * mt->Length;
              Color color = rt->GetPixel(x, y);
              bool boundary = (rt->GetPixel(Math::Min(rt->Width - 1, x + 1), y)).ToArgb() != color.ToArgb() ||
                              (rt->GetPixel(x, Math::Min(rt->Height - 1, y + 1))).ToArgb() != color.ToArgb() ||
                              (rt->GetPixel(Math::Max(0, x - 1), y)).ToArgb() != color.ToArgb() ||
                              (rt->GetPixel(x, Math::Max(0, y - 1))).ToArgb() != color.ToArgb();
              if (!boundary)
              {
                if (mt->BoundChromosome != nullptr)
                {
                  if (color.ToArgb() != COLLISION_KIN_COLOR.ToArgb())
                  { return "Wrong kinetochore attachment"; }
                  kinMTs += 1;
                }
                else if (Math::Abs(mtEnd->Length - r_cell) < 1e-7)
                {
                  if (color.ToArgb() != COLLISION_BACK_COLOR.ToArgb())
                  { return "Wrong collision with cell hull"; }
                  cellMTs += 1;
                }
                else
                {
                  if (color.ToArgb() != COLLISION_HAND_COLOR.ToArgb())
                  { return "Wrong collision with chromosome hand"; }
                  handMTs += 1;
                }
                debugRT->SetPixel(x, y, Color::Red);
              }
            }
            n += 1;
          }
        }

        // For test debugging only.
        debugRT->Save("Rendered.png", System::Drawing::Imaging::ImageFormat::Png);
      }
      finally
      {
        if (ts != nullptr) { delete ts; }
        if (debugRT != nullptr) { delete ts; }
      }
    }
    finally { if (render != nullptr) { delete render; } }
  }
  catch (Exception ^ex) { return ex->Message; }
  finally { Helper::ClearUpTestDirectory(); }

  return nullptr;
}

static inline void CollisionSpiralInitializer(int mtN, int mtsPerPole,
                                              [Out] double %dirX,
                                              [Out] double %dirY,
                                              [Out] double %dirZ)
{
  double a = (double)mtN / (mtsPerPole - 1);
  double c1 = 8 * 2 * Math::PI;
  double c2 = 1.0 / 16;
  auto res = gcnew Vec3(1.0, a * c2 * Math::Sin(a * c1), a * c2 * Math::Cos(a * c1));
  res = res->Normailzed;
  dirX = res->x;
  dirY = res->y;
  dirZ = res->z;
}

static inline SimParams ^CollisionConfig()
{
  auto config = gcnew SimParams();
      
  config[SimParameter::Int::N_Cr_Total] = 1;
  config[SimParameter::Int::N_MT_Total] = 400;
  config[SimParameter::Int::Spring_Brake_MTs] = 1000;
  config[SimParameter::Int::Spring_Brake_Type] = 1;
  config[SimParameter::Double::Spring_Length] = 0.1;
  config[SimParameter::Double::D_Rot] = 0.0;
  config[SimParameter::Double::D_Trans] = 0.0;
  config[SimParameter::Double::F_Cat] = 0.0;
  config[SimParameter::Double::F_Res] = 1.0;
  config[SimParameter::Double::K_On] = 1.0 / config[SimParameter::Double::Dt];
  config[SimParameter::Double::K_Off] = 0.0;
  config[SimParameter::Double::V_Dep] = 0.0;
  config[SimParameter::Double::Const_A] = 0.0;
  config[SimParameter::Double::Const_B] = 0.0;
  config[SimParameter::Double::Cr_Kin_Angle] = 180.0;
      
  return config;
}

TEST(Collision, Centered)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, RotY)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer, 0.0, 0.0, 0.0, 0.0, Math::PI / 2, 0.0);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, ShiftYZ_RotXYZ)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           0.0, 2e-7, 1.5e-7, Math::PI / 3, Math::PI / 5, Math::PI / 7);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, BackPlane)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           1.1e-7, 0.9e-7, 0.0, Math::PI / 10, Math::PI / 10, Math::PI / 2.1);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, Cap)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           0.0, -2.1e-6, -0.9e-6, Math::PI / 9, Math::PI / 11, Math::PI / 6);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, Gap)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           1.1e-7, 0.9e-7, 0.0, 0.0, 0.0, Math::PI / 1.97);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}
