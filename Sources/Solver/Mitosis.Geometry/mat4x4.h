
#pragma

#include <math.h>
#include "vec4.h"

template <class T> class mat3x3;
template <class T> class mat4x4;

template <class T>
inline mat4x4<T> MatrixTranspose(const mat4x4<T> &mat)
{
  mat4x4<T> res;
  res.a[0]  = mat.a[0];
  res.a[1]  = mat.a[4];
  res.a[2]  = mat.a[8];
  res.a[3]  = mat.a[12];
  res.a[4]  = mat.a[1];
  res.a[5]  = mat.a[5];
  res.a[6]  = mat.a[9];
  res.a[7]  = mat.a[13];
  res.a[8]  = mat.a[2];
  res.a[9]  = mat.a[6];
  res.a[10] = mat.a[10];
  res.a[11] = mat.a[14];
  res.a[12] = mat.a[3];
  res.a[13] = mat.a[7];
  res.a[14] = mat.a[11];
  res.a[15] = mat.a[15];
  return res;
}

template <class T>
inline mat4x4<T> Mat4x4Identity()
{
  mat4x4<T> res;
    res.a[0] = 1.0;
    res.a[5] = 1.0;
    res.a[10] = 1.0;
  res.a[15] = 1.0;
    return res;
}

template <class T>
class mat4x4
{
  public:
    T a[16]; // row-based implementation: row1-row2-row3-row4

    // Constructors
      
    inline mat4x4()
    {
      for (int i = 0; i < 16; i++)
        a[i] = 0.0;
    }

    inline mat4x4(const mat4x4<T> &mat)
    {
      for (int i = 0; i < 16; i++)
        a[i] = mat.a[i];
    }

    inline mat4x4(T _11, T _12, T _13, T _14,
            T _21, T _22, T _23, T _24,
            T _31, T _32, T _33, T _34,
            T _41, T _42, T _43, T _44)
    {
      a[0]  = _11;
      a[1]  = _12;
      a[2]  = _13;
      a[3]  = _14;
      a[4]  = _21;
      a[5]  = _22;
      a[6]  = _23;
      a[7]  = _24;
      a[8]  = _31;
      a[9]  = _32;
      a[10] = _33;
      a[11] = _34;
      a[12] = _41;
      a[13] = _42;
      a[14] = _43;
      a[15] = _44;
    }

    // Operations

    inline mat4x4<T> operator +(const mat4x4<T> &mat) const
    {
      mat4x4<T> res;
      for (int i = 0; i < 16; i++)
        res.a[i] = this->a[i] + mat.a[i];
      return res;
    }

    inline mat4x4<T> operator -() const
    {
      mat4x4<T> res;
      for (int i = 0; i < 16; i++)
        res.a[i] = -this->a[i];
      return res;
    }

    inline mat4x4<T> operator -(const mat4x4<T> &mat) const
    {
      mat4x4<T> res;
      for (int i = 0; i < 16; i++)
        res.a[i] = this->a[i] - mat.a[i];
      return res;
    }

    inline mat4x4<T> operator *(T val) const
    {
      mat4x4<T> res;
      for (int i = 0; i < 16; i++)
        res.a[i] = this->a[i] * val;
      return res;
    }
      
    inline vec4<T> operator *(const vec4<T> &vec) const
    {
      return vec4<T>(a[0]*vec.x + a[1]*vec.y + a[2]*vec.z + a[3]*vec.w,
               a[4]*vec.x + a[5]*vec.y + a[6]*vec.z + a[7]*vec.w,
               a[8]*vec.x + a[9]*vec.y + a[10]*vec.z + a[11]*vec.w,
               a[12]*vec.x + a[13]*vec.y + a[14]*vec.z + a[15]*vec.w);
    };
      
    inline mat4x4<T> operator *(const mat4x4<T> &mat) const
    {
      mat4x4<T> res;
      for (int i = 0; i < 16; i++)
      {
        int _i1 = i & (~3);
        int _i2 = i % 4;
        for (int j = 0; j < 4; j++)
          res.a[i] += this->a[_i1 + j] * mat.a[_i2 + j * 4];
      }
      return res;
    }

    inline mat4x4<T> operator /(T val) const
    {
      return (*this) * (1.0 / val);
    }
      
    inline const mat4x4<T> &operator =(const mat4x4<T> &mat)
    {
      for (int i = 0; i < 16; i++)
        this->a[i] = mat.a[i];
      return *this;
    }
};
