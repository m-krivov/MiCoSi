#pragma once
#include "MiCoSi.Core/Defs.h"

#include "mat3x3.h"

class mat3x3f :public mat3x3<float>
{
  public:
    inline mat3x3f()
      :mat3x3<float>()
    { /*nothing*/ }

    inline mat3x3f(const mat3x3<float> &mat)
      :mat3x3<float>(mat)
    { /*nothing*/ }

    inline mat3x3f(float _11, float _12, float _13,
             float _21, float _22, float _23,
             float _31, float _32, float _33)
      :mat3x3<float>(_11, _12, _13,
               _21, _22, _23,
               _31, _32, _33)
    { /*nothing*/ }

  static const mat3x3f ZERO;
  static const mat3x3f IDENTITY;
};

typedef mat3x3_assigner<float> mat3x3f_assigner;

class mat3x3d :public mat3x3<double>
{
  public:
    inline mat3x3d()
      :mat3x3<double>()
    { /*nothing*/ }

    inline mat3x3d(const mat3x3<double> &mat)
      :mat3x3<double>(mat)
    { /*nothing*/ }

    inline mat3x3d(double _11, double _12, double _13,
                   double _21, double _22, double _23,
                   double _31, double _32, double _33)
      :mat3x3<double>(_11, _12, _13,
                _21, _22, _23,
                _31, _32, _33)
    { /*nothing*/ }

  static const mat3x3d ZERO;
  static const mat3x3d IDENTITY;
};

typedef mat3x3_assigner<double> mat3x3d_assigner;

#ifdef MICOSI_PRECISION_FP64
typedef mat3x3d mat3x3r;
typedef mat3x3d_assigner mat3x3r_assigner;
#else
typedef mat3x3f mat3x3r;
typedef mat3x3f_assigner mat3x3r_assigner;
#endif
