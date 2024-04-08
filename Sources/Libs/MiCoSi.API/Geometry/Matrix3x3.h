#pragma once

#include "Converter.h"
#include "Vector3.h"
#include "MiCoSi.Geometry/mat3x3x.h"

namespace MiCoSi
{

// One day, such class will be added to System::Numerics. One day ...
public value class Matrix3x3
{
  public:
    property float default[int, int]
    {
      float get(int row, int col)
      { return Matrix().a[row * 3 + col]; }

      // Do not implement setter: C++/CLI does not provide custom copy constructor
      // So we may share our instance of 'core_' with someone else
    }

    static Matrix3x3()
    {
      zero_     = gcnew Core(mat3x3r::ZERO);
      identity_ = gcnew Core(mat3x3r::IDENTITY);
    }

    static property Matrix3x3 Zero
    {
      Matrix3x3 get()
      { return Matrix3x3(zero_); }
    }

    static property Matrix3x3 Identity
    {
      Matrix3x3 get()
      { return Matrix3x3(identity_); }
    }

    static Matrix3x3 CreateRotationX(float rads)
    { return Matrix3x3(MatrixRotationX((real)rads)); }

    static Matrix3x3 CreateRotationY(float rads)
    { return Matrix3x3(MatrixRotationY((real)rads)); }

    static Matrix3x3 CreateRotationZ(float rads)
    { return Matrix3x3(MatrixRotationZ((real)rads)); }

    static Matrix3x3 CreateRotationXYZ(float x_rads, float y_rads, float z_rads)
    { return Matrix3x3(MatrixRotationXYZ((real)x_rads, (real)y_rads, (real)z_rads)); }

    static Matrix3x3 operator +(Matrix3x3 %m1, Matrix3x3 %m2)
    { return Matrix3x3(m1.Matrix() + m2.Matrix()); }

    static Matrix3x3 operator -(Matrix3x3 %m1, Matrix3x3 %m2)
    { return Matrix3x3(m1.Matrix() - m2.Matrix()); }

    static Matrix3x3 operator *(Matrix3x3 %m1, float val)
    { return Matrix3x3(m1.Matrix() * (real)val); }

    static Vector3 operator *(Matrix3x3 %m, Vector3 %vec)
    { return Converter::ToVector3(m.Matrix() * Converter::ToVec3R(vec)); }

    static Matrix3x3 operator *(Matrix3x3 %m1, Matrix3x3 %m2)
    { return Matrix3x3(m1.Matrix() * m2.Matrix()); }

    static Matrix3x3 operator /(Matrix3x3 %m, float val)
    { return Matrix3x3(m.Matrix() / val); }

  internal:
    Matrix3x3(const mat3x3r *mat)
    { core_ = gcnew Core(mat); }

    Matrix3x3(const mat3x3r &mat)
    { core_ = gcnew Core(mat); }

    const mat3x3r &Matrix()
    {
      // Thanks to C++/CLI, we do not have default constructor
      // So, 'core_' can be equal to 'nullptr'
      return core_ == nullptr ? zero_->Matrix() : core_->Matrix();
    }

  private:
    // Value class does not have destructor and finalizer
    // Such composition (value class <- ref class) allows us to declare them
    ref class Core
    {
      public:
        Core(const mat3x3r *mat)
        { mat_ = mat; local_ = nullptr; }

        Core(const mat3x3r &mat)
        { local_ = new mat3x3r(mat); mat_ = local_; }

        const mat3x3r &Matrix()
        { return *mat_; }

        void SafeRelease()
        {
          if (local_ != nullptr)
          { delete local_; }
          local_ = nullptr;
          mat_ = nullptr;
        }

        !Core()
        { SafeRelease(); }

        ~Core()
        { SafeRelease(); }

      private:
        const mat3x3r *mat_;
        mat3x3r *local_;
    };

    Matrix3x3(Core ^core)
    { core_ = core; }

    Core ^core_;
    static Core ^zero_, ^identity_;
};

} // namespace MiCoSi
