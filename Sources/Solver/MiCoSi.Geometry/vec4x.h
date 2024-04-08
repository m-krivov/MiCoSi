#pragma once
#include "MiCoSi.Core/Defs.h"

#include "vec3.h"
#include "vec4.h"

class vec4f :public vec4<float>
{
  public:
    inline vec4f()
      :vec4<float>()
    { /*nothing*/ }

    inline vec4f(const vec3<float> &vec)
      :vec4<float>(vec)
    { /*nothing*/ }

    inline vec4f(const vec4<float> &vec)
      :vec4<float>(vec)
    { /*nothing*/ }

    inline vec4f(float x, float y, float z, float w)
      :vec4<float>(x, y, z, w)
    { /*nothing*/ }

    inline operator vec4<double>() const
    {
      return vec4<double>(x, y, z, w);
    }
    static const vec4f ZERO;
    static const vec4f DEFAULT_DIRECT;
    static const vec4f DEFAULT_UP;
    static const vec4f DEFAULT_LEFT;
};

class vec4d :public vec4<double>
{
  public:
    inline vec4d()
      :vec4<double>()
    { /*nothing*/ }

    inline vec4d(const vec3<double> &vec)
      :vec4<double>(vec)
    { /*nothing*/ }

    inline vec4d(const vec4<double> &vec)
      :vec4<double>(vec)
    { /*nothing*/ }

    inline vec4d(double x, double y, double z, double w)
      :vec4<double>(x, y, z, w)
    { /*nothing*/ }

    inline operator vec4<float>() const
    {
      return vec4<float>((float)x, (float)y, (float)z, (float)w);
    }

  static const vec4d ZERO;
  static const vec4d DEFAULT_DIRECT;
  static const vec4d DEFAULT_UP;
  static const vec4d DEFAULT_LEFT;
};

#ifdef MICOSI_PRECISION_FP64
typedef vec4d vec4r;
#else
typedef vec4f vec4r;
#endif
