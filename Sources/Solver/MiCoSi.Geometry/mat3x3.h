#pragma once
#include "MiCoSi.Core/Defs.h"

#include "vec3.h"

template <class T> class mat3x3;
template <class T> class mat4x4;

template <class T, class P>
T Constraction(P value)
{
  uint64* uvl = (uint64*)&value;
  uint64 vl = uvl[0];
  vl &= 0xFFFFFFFFE0000000;
  P* ur = (P*)&vl;
  return (T)(ur[0]);  
}

template <class T>
T Sin(T a)
{
#ifdef IDENTICAL
  return (T)Constraction<real, double>(sin((double)a));
#else
  return sin(a);
#endif
}

template <class T>
T Cos(T a)
{
#ifdef IDENTICAL
  return (T)Constraction<real, double>(cos((double)a));
#else
  return cos(a);
#endif
}

template <class T>
inline mat3x3<T> MatrixRotationX(T angle)
{
  mat3x3<T> res;
  T cosa = Cos(angle);
  T sina = Sin(angle);
  res.a[0] = (T)1.0;
  res.a[4] = cosa;
  res.a[5] = -sina;
  res.a[7] = sina;
  res.a[8] = cosa;
  return res;
}

template <class T>
inline mat3x3<T> MatrixRotationY(T angle)
{
  mat3x3<T> res;
  T cosa = Cos(angle);
  T sina = Sin(angle);
  res.a[0] = cosa;
  res.a[2] = sina;
  res.a[4] = (T)1.0;
  res.a[6] = -sina;
  res.a[8] = cosa;
  return res;
}

template <class T>
inline mat3x3<T> MatrixRotationZ(T angle)
{
  mat3x3<T> res;
  T cosa = Cos(angle);
  T sina = Sin(angle);
  res.a[0] = cosa;
  res.a[1] = -sina;
  res.a[3] = sina;
  res.a[4] = cosa;
  res.a[8] = (T)1.0;
  return res;
}

template <class T>
inline mat3x3<T> MatrixRotationXYZ(T x, T y, T z)
{
  return MatrixRotationZ(z) *
         MatrixRotationY(y) *
         MatrixRotationX(x);
}

// Creates rotation matrix around axis for angle, which cosinus is equal to "cosa".
// Note: angle must belong to [0, Pi].
template <class T>
inline mat3x3<T> MatrixRotationAxisCos(const vec3<T> &axis, T cosa)
{
  mat3x3<T> res;
  T c = cosa;
  T s = sqrt(1 - c * c);
  T ic = 1 - c;
  res.a[0] = axis.x * axis.x * ic + c;
  res.a[1] = axis.x * axis.y * ic - axis.z * s;
  res.a[2] = axis.x * axis.z * ic + axis.y * s;
  res.a[3] = axis.x * axis.y * ic + axis.z * s;
  res.a[4] = axis.y * axis.y * ic + c;
  res.a[5] = axis.y * axis.z * ic - axis.x * s;
  res.a[6] = axis.x * axis.z * ic - axis.y * s;
  res.a[7] = axis.y * axis.z * ic + axis.x * s;
  res.a[8] = axis.z * axis.z * ic + c;
  return res;
}

// Creates rotation matrix around axis.
// Note: angle must belong to [0, 2 * Pi].
template <class T>
inline mat3x3<T> MatrixRotationAxis(const vec3<T> &axis, T angle)
{
  mat3x3<T> res;
  T s = sin(angle);
  T c = cos(angle);
  T ic = 1 - c;
  res.a[0] = axis.x * axis.x * ic + c;
  res.a[1] = axis.x * axis.y * ic - axis.z * s;
  res.a[2] = axis.x * axis.z * ic + axis.y * s;
  res.a[3] = axis.x * axis.y * ic + axis.z * s;
  res.a[4] = axis.y * axis.y * ic + c;
  res.a[5] = axis.y * axis.z * ic - axis.x * s;
  res.a[6] = axis.x * axis.z * ic - axis.y * s;
  res.a[7] = axis.y * axis.z * ic + axis.x * s;
  res.a[8] = axis.z * axis.z * ic + c;
  return res;
}

//Creates matrix, which rotates v1 into v2.
template <class T>
inline mat3x3<T> MatrixRotationVectors(const vec3<T> &v1, const vec3<T> &v2)
{
  vec3<T> nv1 = v1.Normalize();
  vec3<T> nv2 = v2.Normalize();
  T cosa = DotProduct(nv1, nv2);
  vec3<T> axis = CrossProduct(nv1, nv2).Normalize();
  return MatrixRotationAxisCos(axis, cosa);;
}

template <class T>
inline mat3x3<T> MatrixTranspose(const mat3x3<T> &mat)
{
  mat3x3<T> res;
  res.a[0] = mat.a[0];
  res.a[1] = mat.a[3];
  res.a[2] = mat.a[6];
  res.a[3] = mat.a[1];
  res.a[4] = mat.a[4];
  res.a[5] = mat.a[7];
  res.a[6] = mat.a[2];
  res.a[7] = mat.a[5];
  res.a[8] = mat.a[8];
  return res;
}

template <class T>
inline mat3x3<T> Mat3x3Identity()
{
  mat3x3<T> res;
  res.a[0] = (T)1.0;
  res.a[4] = (T)1.0;
  res.a[8] = (T)1.0;
  return res;
}

template <class T>
class mat3x3
{
  public:
    T a[9]; // row-based implementation: row1-row2-row3

    // Constructors

    inline mat3x3()
    {
      for (int i = 0; i < 9; i++)
      { a[i] = (T)0.0; }
    }

    inline mat3x3(const mat3x3<T> &mat)
    {
      for (int i = 0; i < 9; i++)
      { a[i] = mat.a[i]; }
    }

    inline mat3x3(T _11, T _12, T _13,
                  T _21, T _22, T _23,
                  T _31, T _32, T _33)
    {
      a[0] = _11;
      a[1] = _12;
      a[2] = _13;
      a[3] = _21;
      a[4] = _22;
      a[5] = _23;
      a[6] = _31;
      a[7] = _32;
      a[8] = _33;
    }

    //Operations

    inline mat3x3<T> operator +(const mat3x3<T> &mat) const
    {
      mat3x3<T> res;
      for (int i = 0; i < 9; i++)
      { res.a[i] = a[i] + mat.a[i]; }
      return res;
    }

    inline mat3x3<T> operator -(const mat3x3<T> &mat) const
    {
      mat3x3<T> res;
      for (int i = 0; i < 9; i++)
      { res.a[i] = a[i] - mat.a[i]; }
      return res;
    }

    inline mat3x3<T> operator *(T val) const
    {
      mat3x3<T> res;
      for (int i = 0; i < 9; i++)
      { res.a[i] = a[i] * val; }
      return res;
    }

    inline vec3<T> operator *(const vec3<T> &vec) const
    {
      return vec3<T>(a[0] * vec.x + a[1] * vec.y + a[2] * vec.z,
                     a[3] * vec.x + a[4] * vec.y + a[5] * vec.z,
                     a[6] * vec.x + a[7] * vec.y + a[8] * vec.z);
    };

    inline mat3x3<T> operator *(const mat3x3<T> &mat) const
    {
      mat3x3<T> res;
      res.a[0] = a[0] * mat.a[0] +
                 a[1] * mat.a[3] +
                 a[2] * mat.a[6];
      res.a[1] = a[0] * mat.a[1] +
                 a[1] * mat.a[4] +
                 a[2] * mat.a[7];
      res.a[2] = a[0] * mat.a[2] +
                 a[1] * mat.a[5] +
                 a[2] * mat.a[8];
      res.a[3] = a[3] * mat.a[0] +
                 a[4] * mat.a[3] +
                 a[5] * mat.a[6];
      res.a[4] = a[3] * mat.a[1] +
                 a[4] * mat.a[4] +
                 a[5] * mat.a[7];
      res.a[5] = a[3] * mat.a[2] +
                 a[4] * mat.a[5] +
                 a[5] * mat.a[8];
      res.a[6] = a[6] * mat.a[0] +
                 a[7] * mat.a[3] +
                 a[8] * mat.a[6];
      res.a[7] = a[6] * mat.a[1] +
                 a[7] * mat.a[4] +
                 a[8] * mat.a[7];
      res.a[8] = a[6] * mat.a[2] +
                 a[7] * mat.a[5] +
                 a[8] * mat.a[8];
      return res;
    }

    inline mat3x3<T> operator /(T val) const
    {
      return (*this) * ((T)1.0 / val);
    }

    inline mat3x3<T> &operator =(const mat3x3<T> &mat)
    {
      for (int i = 0; i < 9; i++)
      { a[i] = mat.a[i]; }
      return *this;
    }
};

template <class T>
class mat3x3_assigner
{
  private:
    T *_pa[9];

  public:
    inline mat3x3_assigner(T *a1, T *a2, T *a3,
                 T *a4, T *a5, T *a6,
                 T *a7, T *a8, T *a9)
    {
      _pa[0] = a1; _pa[1] = a2; _pa[2] = a3;
      _pa[3] = a4; _pa[4] = a5; _pa[5] = a6;
      _pa[6] = a7; _pa[7] = a8; _pa[8] = a9;
    }

    inline const mat3x3<T> &operator =(const mat3x3<T> &mat)
    {
      for (int i = 0; i < 9; i++)
        *(_pa[i]) = mat.a[i];
      return mat;
    }

    inline operator mat3x3<T>()
    {
      return mat3x3<T>(*(_pa[0]), *(_pa[1]), *(_pa[2]),
               *(_pa[3]), *(_pa[4]), *(_pa[5]),
               *(_pa[6]), *(_pa[7]), *(_pa[8]));
    }
};
