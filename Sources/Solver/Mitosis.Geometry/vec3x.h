
#pragma once

#include "Mitosis.Core/Defs.h"
#include "vec3.h"
#include "vec4.h"

class vec3f :public vec3<float>
{
  public:
    inline vec3f()
      :vec3<float>()
    { /*nothing*/ }

    inline vec3f(const vec3<float> &vec)
      :vec3<float>(vec)
    { /*nothing*/ }

    inline vec3f(const vec4<float> &vec)
      :vec3<float>(vec)
    { /*nothing*/ }

    inline vec3f(float x, float y, float z)
      :vec3<float>(x, y, z)
    { /*nothing*/ }

    inline operator vec3<double>() const
    {
      return vec3<double>(x, y, z);
    }

    static const vec3f ZERO;
    static const vec3f DEFAULT_DIRECT;
    static const vec3f DEFAULT_UP;
    static const vec3f DEFAULT_LEFT;
};

typedef vec3_assigner<float> vec3f_assigner;

class vec3d :public vec3<double>
{
  public:
    inline vec3d()
      :vec3<double>()
    { /*nothing*/ }

    inline vec3d(const vec3<double> &vec)
      :vec3<double>(vec)
    { /*nothing*/ }

    inline vec3d(const vec4<double> &vec)
      :vec3<double>(vec)
    { /*nothing*/ }

    inline vec3d(double x, double y, double z)
      :vec3<double>(x, y, z)
    { /*nothing*/ }

    inline operator vec3<float>() const
    {
      return vec3<float>((float)x, (float)y, (float)z);
    }

    static const vec3d ZERO;
    static const vec3d DEFAULT_DIRECT;
    static const vec3d DEFAULT_UP;
    static const vec3d DEFAULT_LEFT;
};

typedef vec3_assigner<double> vec3d_assigner;

#ifdef DOUBLE_PRECISION
typedef vec3d vec3r;
typedef vec3d_assigner vec3r_assigner;
#else
typedef vec3f vec3r;
typedef vec3f_assigner vec3r_assigner;
#endif
