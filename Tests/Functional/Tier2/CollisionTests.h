#pragma once
#include "Defs.h"
#include "Helpers.h"

using System::Drawing::Bitmap;
using System::Drawing::Color;
using System::Drawing::Graphics;

public ref class CollisionRender : public IDisposable
{
  private:
    double _dl, _da;
    cli::array<double, 2> ^_depthBuffer;
    Vector3 _screenNormal;
    double _coeffD;

    void ComputeCoeffs()
    {
      _screenNormal = Vector3::Cross(ScreenUpVector, ScreenRightVector);
      Vector3 tmp = _screenNormal * ScreenAnchorPoint;
      _coeffD = -(tmp.X + tmp.Y + tmp.Z);
    }
    
  public:
    property Bitmap ^RenderTarget;
    property Vector3 ScreenAnchorPoint;
    property Vector3 ScreenUpVector;
    property Vector3 ScreenRightVector;
    property Vector3 ViewPoint;

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

      ViewPoint = Vector3(-1.0f, 0.0f, 0.0f);
      ScreenAnchorPoint = Vector3(-0.25f, 0.5f, -0.5f);
      ScreenUpVector = Vector3(0.0f, -1.0f, 0.0f);
      ScreenRightVector = Vector3(0.0f, 0.0f, 1.0f);
    }

    void Clear(Color color)
    {
      Graphics ^gr = Graphics::FromImage(RenderTarget);
      gr->Clear(color);
      delete gr;
    }

    Tuple<int, int> ^Project(Vector3 point)
    {
      Vector3 p1 = ViewPoint;
      Vector3 dir = point + (-p1);

      Vector3 tmp1 = _screenNormal * p1;
      Vector3 tmp2 = _screenNormal * dir;
      double t = (-_coeffD - tmp1.X - tmp1.Y - tmp1.Z) / (tmp2.X + tmp2.Y + tmp2.Z);
      Vector3 projPoint = p1 + dir * (float)t;

      int x = (int)Math::Round(Vector3::Dot(projPoint + (-ScreenAnchorPoint),
                                            Vector3::Normalize(ScreenRightVector))
                  / ScreenRightVector.Length() * RenderTarget->Width, 0);
      int y = (int)Math::Round(Vector3::Dot(projPoint + (-ScreenAnchorPoint),
                                            Vector3::Normalize(ScreenUpVector))
                  / ScreenUpVector.Length() * RenderTarget->Height);
      
      return gcnew Tuple<int, int>(x, y);
    }

    void ProjectAndDraw(Vector3 point, Color color)
    {
      auto pp = Project(point);
      int x = pp->Item1;
      int y = pp->Item2;
      double depth = (point + (-ViewPoint)).Length();
      if (0 <= x && x < RenderTarget->Width &&
          0 <= y && y < RenderTarget->Height &&
          _depthBuffer[x, y] > depth)
      {
        RenderTarget->SetPixel(x, y, color);
        _depthBuffer[x, y] = depth;
      }
    }

    void FillRectangle(Vector3 point, Vector3 v1, Vector3 v2, Color color)
    {
      ComputeCoeffs();

      float v1Len = v1.Length(), v2Len = v2.Length();
      Vector3 nv1 = Vector3::Normalize(v1), nv2 = Vector3::Normalize(v2);
      float sv1 = 0.0f, sv2 = 0.0f;
      while (sv2 < v2Len)
      {
        while (sv1 < v1Len)
        {
          ProjectAndDraw(point + nv1 * sv1 + nv2 * sv2, color);
          sv1 += (float)_dl;
        }
        sv1 = 0.0;
        sv2 += (float)_dl;
      }
    }

    void FillArc(Vector3 centerPoint, Vector3 fromVec, Vector3 toVec, Color color)
    {
      ComputeCoeffs();

      float cl = 0.0f, ca = 0.0f;
      float lmax = fromVec.Length();
      float amax = (float)Math::Acos(Vector3::Dot(Vector3::Normalize(fromVec), Vector3::Normalize(toVec)));
      Vector3 ortVec = Vector3(0.0, 0.0, 0.0) +
                               (-Vector3::Normalize(Vector3::Cross(fromVec, Vector3::Cross(fromVec, toVec))));
      Vector3 nfromVec = Vector3::Normalize(fromVec);
      while (cl < lmax)
      {
        while (ca < amax)
        {
          ProjectAndDraw(centerPoint
                         + (nfromVec * (float)Math::Cos(ca) + ortVec * (float)Math::Sin(ca)) * cl,
                         color);
          ca += (float)_da;
        }
        ca = 0.0f;
        cl += (float)_dl;
      }
    }

    void FillSemiCylinder(Vector3 centerPoint, Vector3 fromVec, Vector3 toVec, Vector3 axis, Color color)
    {
      ComputeCoeffs();

      double cl = 0.0, ca = 0.0;
      double lmax = axis.Length();
      double amax = Math::Acos(Vector3::Dot(Vector3::Normalize(fromVec),
                                            Vector3::Normalize(toVec)));
      Vector3 ortVec = Vector3(0.0, 0.0, 0.0) +
                       (-Vector3::Normalize(Vector3::Cross(fromVec, Vector3::Cross(fromVec, toVec))) * fromVec.Length());
      Vector3 naxis = Vector3::Normalize(axis);
      while (cl < lmax)
      {
        while (ca < amax)
        {
          ProjectAndDraw(centerPoint
                         + fromVec * (float)Math::Cos(ca)
                         + ortVec * (float)Math::Sin(ca)
                         + naxis * (float)cl, color);
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
  float l_poles = (float)(config[SimParameter::Double::L_Poles] * 1e-6);
  render->ViewPoint = Vector3(-l_poles / 2, 0.0f, 0.0f);
  render->ScreenAnchorPoint = Vector3(-l_poles / 4, l_poles / 16, -l_poles / 16);
  render->ScreenUpVector = Vector3(0.0f, -l_poles / 8, 0.0f);
  render->ScreenRightVector = Vector3(0.0f, 0.0f, l_poles / 8);
}

static inline void CollisionMagicQuaterCylinder(CollisionRender ^render,
                                                Vector3 rootPoint, Vector3 axisVec,
                                                Vector3 fromVec, Vector3 toVec,
                                                Color plainColor, Color curveColor)
{
  render->FillRectangle(rootPoint, fromVec, axisVec, plainColor);
  render->FillRectangle(rootPoint, toVec, axisVec, plainColor);
  render->FillArc(rootPoint, fromVec, toVec, curveColor);
  render->FillArc(rootPoint + axisVec, fromVec, toVec, curveColor);
  render->FillSemiCylinder(rootPoint, fromVec, toVec, axisVec, curveColor);
}

static inline void CollisionTwoMagicHalfCylinders(CollisionRender ^render,
                                                  Vector3 centerPoint, float length, float offset, float d,
                                                  Matrix3x3 orient,
                                                  Color plainColor, Color curveColor)
{
  auto toUp = orient * Vector3(0.0f, length / 2, 0.0f);
  auto toLeft = orient * Vector3(-offset / 2, 0.0f, 0.0f);

  CollisionMagicQuaterCylinder(render, centerPoint + toLeft + toUp,
                               toUp * (-2.0f),
                               orient * Vector3(0.0f, 0.0f, -d / 2.0f),
                               orient * Vector3(-d / 2.0f, 0.0f, 0.0f),
                               plainColor, curveColor);
  CollisionMagicQuaterCylinder(render, centerPoint + toLeft + toUp,
                               toUp * (-2.0f),
                               orient * Vector3(-d / 2.0f, 0.0f, 0.0f),
                               orient * Vector3(0.0f, 0.0f, d / 2.0f),
                               plainColor, curveColor);

  CollisionMagicQuaterCylinder(render, centerPoint + (-toLeft) + toUp,
                               toUp * (-2.0f),
                               orient * Vector3(0.0f, 0.0f, -d / 2.0f),
                               orient * Vector3(d / 2.0f, 0.0f, 0.0f),
                               plainColor, curveColor);
  CollisionMagicQuaterCylinder(render, centerPoint + (-toLeft) + toUp,
                               toUp * (-2.0f),
                               orient * Vector3(d / 2.0f, 0.0f, 0.0f),
                               orient * Vector3(0.0f, 0.0f, d / 2.0f),
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
  auto orient = Matrix3x3::CreateRotationXYZ((float)orientation_x,
                                             (float)orientation_y,
                                             (float)orientation_z);
  auto position = Vector3((float)position_x,
                          (float)position_y,
                          (float)position_z);
  float cr_l = (float)parameters->Config[SimParameter::Double::Cr_L] * 1e-6f;
  float cr_kin_l = (float)parameters->Config[SimParameter::Double::Cr_Kin_L] * 1e-6f;
  float cr_kin_d = (float)parameters->Config[SimParameter::Double::Cr_Kin_D] * 1e-6f;
  float cr_hand_l = (cr_l - cr_kin_l) / 2.0f;
  float cr_hand_d = (float)parameters->Config[SimParameter::Double::Cr_Hand_D] * 1e-6f;
  float spring_length = (float)parameters->Config[SimParameter::Double::Spring_Length] * 1e-6f;

  CollisionTwoMagicHalfCylinders(render, position + orient * Vector3(0.0f, (cr_hand_l + cr_kin_l) / 2.0f, 0.0f),
                                 cr_hand_l, spring_length, cr_hand_d, orient,
                                 COLLISION_HAND_COLOR, COLLISION_HAND_COLOR);
  CollisionTwoMagicHalfCylinders(render, position + orient * Vector3(0.0f, -(cr_hand_l + cr_kin_l) / 2.0f, 0.0f),
                                 cr_hand_l, spring_length, cr_hand_d, orient,
                                 COLLISION_HAND_COLOR, COLLISION_HAND_COLOR);
  CollisionTwoMagicHalfCylinders(render, position, cr_kin_l, spring_length, cr_kin_d, orient,
                                 COLLISION_HAND_COLOR, COLLISION_KIN_COLOR);
}

static inline  String ^CollisionTest(SimParams ^config,
                                     void (*initializer)(int mtN, int mtsPerPole,
                                                         [Out] float %dirX,
                                                         [Out] float %dirY,
                                                         [Out] float %dirZ),
                                     float px, float py, float pz,
                                     float ox, float oy, float oz)
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
        float dirX = 0.0, dirY = 0.0, dirZ = 0.0;

        initializer(i, mtsPerPole, dirX, dirY, dirZ);
        parameters->InitialStates->AddMT(PoleType::Left, dirX, dirY, dirZ, 0.0, MTState::Polymerization);
        parameters->InitialStates->AddMT(PoleType::Right, 0.0, 1.0, 0.0, 0.0, MTState::Polymerization);
        lMTs[i] = render->Project(Vector3(dirX, dirY, dirZ));
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
              auto mtEnd = Vector3(cell->GetPole(PoleType::Left)->Position) +
                           Vector3(mt->Direction) * (float)mt->Length;
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
                else if (Math::Abs(mtEnd.Length() - r_cell) < 1e-7)
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
                                              [Out] float %dirX,
                                              [Out] float %dirY,
                                              [Out] float %dirZ)
{
  float a = (float)mtN / (mtsPerPole - 1);
  float c1 = 8 * 2 * (float)Math::PI;
  float c2 = 1.0f / 16;
  auto res = Vector3(1.0f, a * c2 * (float)Math::Sin(a * c1), a * c2 * (float)Math::Cos(a * c1));
  res = Vector3::Normalize(res);
  dirX = res.X;
  dirY = res.Y;
  dirZ = res.Z;
}

static inline SimParams ^CollisionConfig()
{
  auto config = gcnew SimParams();
      
  config[SimParameter::Int::N_Cr_Total] = 1;
  config[SimParameter::Int::N_MT_Total] = 400;
  config[SimParameter::Int::N_KMT_Max] = 1000;
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
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, RotY)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           0.0f, 0.0f, 0.0f, 0.0f, (float)Math::PI / 2, 0.0f);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, ShiftYZ_RotXYZ)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           0.0f, 2e-7f, 1.5e-7f,
                           (float)Math::PI / 3, (float)Math::PI / 5, (float)Math::PI / 7);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, BackPlane)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           1.1e-7f, 0.9e-7f, 0.0f,
                           (float)Math::PI / 10, (float)Math::PI / 10, (float)Math::PI / 2.1f);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, Cap)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           0.0f, -2.1e-6f, -0.9e-6f,
                           (float)Math::PI / 9, (float)Math::PI / 11, (float)Math::PI / 6);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}

TEST(Collision, Gap)
{
  auto config = CollisionConfig();
  auto ret = CollisionTest(config, CollisionSpiralInitializer,
                           1.1e-7f, 0.9e-7f, 0.0f, 0.0f, 0.0f, (float)Math::PI / 1.97f);
  if (!String::IsNullOrEmpty(ret))
  { FAIL() << StringToString(ret); }
}
