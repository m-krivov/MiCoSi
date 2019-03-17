#pragma once
#include "Defs.h"

static inline std::string StringToString(String ^str)
{
  std::string res;
  if (str->Length != 0)
  {
    res.resize(str->Length);
    for (int i = 0; i < str->Length; i++)
    {
      res[i] = (char)str[i];
    }
  }
  return res;
}

public ref class Vec3
{
  public:
    property double x;
    property double y;
    property double z;

    Vec3() { x = 0.0; y = 0.0; z = 0.0; }
    Vec3(double x, double y, double z) { this->x = x; this->y = y; this->z = z; }
    Vec3(Vec3D ^vec) { x = vec->X; y = vec->Y; z = vec->Z; }

    property double Length
    { double get() { return Math::Sqrt(x * x + y * y + z * z); } }

    property Vec3 ^Normailzed
    {
      Vec3 ^get()
      {
        double len = Length;
        return gcnew Vec3(x / len, y / len, z / len);
      }
    }

    static double DotProduct(Vec3 ^v1, Vec3 ^v2)
    { return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z; }

    static Vec3 ^CrossProduct(Vec3 ^v1, Vec3 ^v2)
    {
      return gcnew Vec3(v1->y * v2->z - v1->z * v2->y,
                        v1->z * v2->x - v1->x * v2->z,
                        v1->x * v2->y - v1->y * v2->x);
    }

    static Vec3 ^operator +(Vec3 ^vec1, Vec3 ^vec2)
    { return gcnew Vec3(vec1->x + vec2->x, vec1->y + vec2->y, vec1->z + vec2->z); }

    static Vec3 ^operator -(Vec3 ^vec1, Vec3 ^vec2)
    { return gcnew Vec3(vec1->x - vec2->x, vec1->y - vec2->y, vec1->z - vec2->z); }

    static Vec3 ^operator *(Vec3 ^vec1, Vec3 ^vec2)
    { return gcnew Vec3(vec1->x * vec2->x, vec1->y * vec2->y, vec1->z * vec2->z); }

    static Vec3 ^operator *(Vec3 ^vec1, double val)
    { return gcnew Vec3(vec1->x * val, vec1->y * val, vec1->z * val); }

    static Vec3 ^operator /(Vec3 ^vec1, double val)
    { return gcnew Vec3(vec1->x / val, vec1->y / val, vec1->z / val); }

    static Vec3 ^operator -(Vec3 ^vec)
    { return gcnew Vec3(-vec->x, -vec->y, -vec->z); }
};

public ref class Matrix3x3
{
  private:
    cli::array<double> ^_a = gcnew cli::array<double>(9);

    Matrix3x3()
    {
      _a[0] = 1.0;
      _a[4] = 1.0;
      _a[8] = 1.0;
    }

    Matrix3x3(Mat3x3D mat)
    {
      for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
          _a[y * 3 + x] = mat[y, x];
    }

    static Matrix3x3 ^operator *(Matrix3x3 ^mat1, Matrix3x3 ^mat2)
    {
      auto res = gcnew Matrix3x3();

      res->_a[0] = mat1->_a[0] * mat2->_a[0] +
                    mat1->_a[1] * mat2->_a[3] +
                    mat1->_a[2] * mat2->_a[6];
      res->_a[1] = mat1->_a[0] * mat2->_a[1] +
                    mat1->_a[1] * mat2->_a[4] +
                    mat1->_a[2] * mat2->_a[7];
      res->_a[2] = mat1->_a[0] * mat2->_a[2] +
                    mat1->_a[1] * mat2->_a[5] +
                    mat1->_a[2] * mat2->_a[8];
      res->_a[3] = mat1->_a[3] * mat2->_a[0] +
                    mat1->_a[4] * mat2->_a[3] +
                    mat1->_a[5] * mat2->_a[6];
      res->_a[4] = mat1->_a[3] * mat2->_a[1] +
                    mat1->_a[4] * mat2->_a[4] +
                    mat1->_a[5] * mat2->_a[7];
      res->_a[5] = mat1->_a[3] * mat2->_a[2] +
                    mat1->_a[4] * mat2->_a[5] +
                    mat1->_a[5] * mat2->_a[8];
      res->_a[6] = mat1->_a[6] * mat2->_a[0] +
                    mat1->_a[7] * mat2->_a[3] +
                    mat1->_a[8] * mat2->_a[6];
      res->_a[7] = mat1->_a[6] * mat2->_a[1] +
                    mat1->_a[7] * mat2->_a[4] +
                    mat1->_a[8] * mat2->_a[7];
      res->_a[8] = mat1->_a[6] * mat2->_a[2] +
                    mat1->_a[7] * mat2->_a[5] +
                    mat1->_a[8] * mat2->_a[8];

      return res;
    }

    static Matrix3x3 ^RotationX(double angle)
    {
      auto res = gcnew Matrix3x3();

      double cosa = Math::Cos(angle);
      double sina = Math::Sin(angle);
      res->_a[0] = 1.0;
      res->_a[4] = cosa;
      res->_a[5] = -sina;
      res->_a[7] = sina;
      res->_a[8] = cosa;

      return res;
    }

    static Matrix3x3 ^RotationY(double angle)
    {
      auto res = gcnew Matrix3x3();

      double cosa = Math::Cos(angle);
      double sina = Math::Sin(angle);
      res->_a[0] = cosa;
      res->_a[2] = sina;
      res->_a[4] = 1.0;
      res->_a[6] = -sina;
      res->_a[8] = cosa;

      return res;
    }

    static Matrix3x3 ^RotationZ(double angle)
    {
      Matrix3x3 ^res = gcnew Matrix3x3();

      double cosa = Math::Cos(angle);
      double sina = Math::Sin(angle);
      res->_a[0] = cosa;
      res->_a[1] = -sina;
      res->_a[3] = sina;
      res->_a[4] = cosa;
      res->_a[8] = 1.0;

      return res;
    }

    static Matrix3x3 ^RotationXYZ(double rot_x, double rot_y, double rot_z)
    {
      return RotationZ(rot_z) * RotationY(rot_y) * RotationX(rot_x);
    }

    Vec3 ^Rotate(Vec3 ^vec)
    {
      return gcnew Vec3(_a[0] * vec->x + _a[1] * vec->y + _a[2] * vec->z,
                        _a[3] * vec->x + _a[4] * vec->y + _a[5] * vec->z,
                        _a[6] * vec->x + _a[7] * vec->y + _a[8] * vec->z);
    }
};

public ref class Helper
{
  public:
    static property String ^WorkingDirectory
    {
      String ^get() { return Directory::GetCurrentDirectory(); }
    }

    static property String ^TestDirectory
    {
      String ^get() { return Path::Combine(WorkingDirectory, "Tests"); }
    }

    static property String ^SimulatorFile
    {
      String ^get() { return Path::Combine(WorkingDirectory, "mitosis.exe"); }
    }

    static void PrepareTestDirectory()
    {
      String ^dir = TestDirectory;
      if (Directory::Exists(dir)) Directory::Delete(dir, true);
      Directory::CreateDirectory(dir);
    }

    static void ClearUpTestDirectory()
    {
      String ^dir = TestDirectory;
      if (Directory::Exists(dir)) Directory::Delete(dir, true);
    }

    static IntPtr CopyData(Cell ^cell)
    {
      int dataSize = cell->Data->DataSize;
      auto res = Marshal::AllocHGlobal(dataSize + 12);
      Int32 *p = (Int32 *)(void *)res;
      p[0] = dataSize;
      p[1] = cell->Data->MTsPerPole;
      p[2] = cell->Data->ChromosomePairs;
      for (int i = 0; i < dataSize / 4; i++)
        p[3 + i] = ((Int32 *)(void *)cell->Data->DataPointer)[i];
      return res;
    }

    static bool CompareData(IntPtr cell1, IntPtr cell2)
    {
      Int32 *p1 = (Int32 *)(void *)cell1;
      Int32 *p2 = (Int32 *)(void *)cell2;
      if (p1[0] != p2[0] || p1[0] <= 0)
        return false;
      for (int i = 1; i < p1[0] / 4 + 2; i++)
        if (p1[i] != p2[i])
          return false;
      return true;
    }

    static void ReleaseData(IntPtr data)
    { Marshal::FreeHGlobal(data); }

    static LaunchResult ^Launch(LaunchParameters ^parameters)
    {
      Launcher launcher(Helper::TestDirectory, Helper::SimulatorFile, parameters);
      return launcher.StartAndWait();
    }

    static TimeStream ^LaunchAndOpen(LaunchParameters ^parameters)
    {
      if (parameters->Args->CellCount != 1)
      { throw gcnew ApplicationException("Test error - cannot open multiple files"); }

      Launcher launcher(Helper::TestDirectory, Helper::SimulatorFile, parameters);
      auto res = launcher.StartAndWait();
      if (res->ExitedWithError)
      {
        throw gcnew ApplicationException(String::Format("Failed to launch the simulation process ({0}: {1})",
                                                        res->ExitCode, res->Output));
      }
      String ^resFile = launcher.Params->Args->CellFile;
      resFile = Path::IsPathRooted(resFile) ? resFile : Path::Combine(launcher.WorkingDir, resFile);
      return TimeStream::Open(resFile);
    }
};

// Assertion 'FAIL()' can be used only in test or test fixture
// So, we are forced to define this helper as macro
#define UNIFIED_TEST(parameters, checker, arg)          \
do {                                                    \
  TimeStream ^ts = nullptr;                             \
  try                                                   \
  {                                                     \
    Helper::PrepareTestDirectory();                     \
    ts = Helper::LaunchAndOpen(parameters);             \
    checker(ts, arg);                                   \
  }                                                     \
  catch (Exception ^ex)                                 \
  { FAIL() << StringToString(ex->Message); }            \
  finally                                               \
  {                                                     \
    if (ts != nullptr) { delete ts; }                   \
    Helper::ClearUpTestDirectory();                     \
  }                                                     \
} while (false)
