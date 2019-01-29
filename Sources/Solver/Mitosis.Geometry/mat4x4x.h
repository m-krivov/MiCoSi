
#pragma once

#include "mat4x4.h"

class mat4x4f :public mat4x4<float>
{
  public:
    inline mat4x4f()
      :mat4x4<float>()
    { /*nothing*/ }

    inline mat4x4f(const mat4x4<float> &mat)
      :mat4x4<float>(mat)
    { /*nothing*/ }

    inline mat4x4f(float _11, float _12, float _13, float _14,
             float _21, float _22, float _23, float _24,
             float _31, float _32, float _33, float _34,
             float _41, float _42, float _43, float _44)
      :mat4x4<float>(_11, _12, _13, _14,
               _21, _22, _23, _24,
               _31, _32, _33, _34,
               _41, _42, _43, _44)
    { /*nothing*/ }

  static const mat4x4f ZERO;
  static const mat4x4f IDENTITY;
};

class mat4x4d :public mat4x4<double>
{
  public:
    inline mat4x4d()
      :mat4x4<double>()
    { /*nothing*/ }

    inline mat4x4d(const mat4x4<double> &mat)
      :mat4x4<double>(mat)
    { /*nothing*/ }

    inline mat4x4d(double _11, double _12, double _13, double _14,
             double _21, double _22, double _23, double _24,
             double _31, double _32, double _33, double _34,
             double _41, double _42, double _43, double _44)
      :mat4x4<double>(_11, _12, _13, _14,
                _21, _22, _23, _24,
                _31, _32, _33, _34,
                _41, _42, _43, _44)
    { /*nothing*/ }

  static const mat4x4d ZERO;
  static const mat4x4d IDENTITY;
};

#ifdef DOUBLE_PRECISION
typedef mat4x4d mat4x4r;
#else
typedef mat4x4f mat4x4r;
#endif
