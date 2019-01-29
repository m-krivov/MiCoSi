
#pragma once

template <class T> class vec3;
template <class T> class vec4;

template <class T>
inline T DotProduct(const vec4<T> &a, const vec4<T> &b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template <class T>
inline vec4<T> CrossProduct(const vec4<T> &a, const vec4<T> &b)
{
  return vec4<T>(a.y * b.z - a.z * b.y,
                   -(a.x * b.z - a.z * b.x),
                   a.x * b.y - a.y * b.x,
           (T)1.0);
}

template <class T>
class vec4
{
  private:
    T vals[4];

  public:
    T &x;
    T &y;
    T &z;
    T &w;

    // Constructors

    inline vec4<T>(): x(vals[0]), y(vals[1]), z(vals[2]), w(vals[3])
    {
      this->x = (T)0.0;
      this->y = (T)0.0;
      this->z = (T)0.0;
      this->w = (T)1.0;
    }

    inline vec4<T>(const vec4<T> &vec): x(vals[0]), y(vals[1]), z(vals[2]), w(vals[3])
    {
      this->x = vec.x;
      this->y = vec.y;
      this->z = vec.z;
      this->w = vec.w;
    }

    inline vec4<T>(const vec3<T> &vec): x(vals[0]), y(vals[1]), z(vals[2]), w(vals[3])
    {
      this->x = vec.x;
      this->y = vec.y;
      this->z = vec.z;
      this->w = (T)1.0;
    }
      
    inline vec4<T>(T x, T y, T z, T w): x(vals[0]), y(vals[1]), z(vals[2]), w(vals[3])
    {
      this->x = x;
      this->y = y;
      this->z = z;
      this->w = w;
    }

    // Operators
      
    inline vec4<T> operator -() const
    {
      return vec4<T>(-this->x,
               -this->y,
               -this->z,
               -this->w);
    }
      
    inline vec4<T> operator -(const vec4<T> &vec) const
    {
      return vec4<T>(this->x - vec.x,
               this->y - vec.y,
               this->z - vec.z,
               this->w - vec.w);
    }
      
    inline vec4<T> operator +(const vec4<T> &vec) const
    {
      return vec4<T>(this->x + vec.x,
               this->y + vec.y,
               this->z + vec.z,
               this->w + vec.w);
    }
      
    template <class T2>
    inline vec4<T> operator *(T2 a) const
    {
      return vec4<T>(this->x * (T)a,
               this->y * (T)a,
               this->z * (T)a,
               this->w * (T)a);
    }
      
    inline vec4<T> operator *(const vec4<T> &vec) const
    {
      return vec4<T>(this->x * vec.x,
               this->y * vec.y,
               this->z * vec.z,
               this->w * vec.w);
    }
      
    template <class T2>
    inline vec4<T> operator /(T2 a) const
    {
      return vec4<T>(this->x / (T)a,
               this->y / (T)a,
               this->z / (T)a,
               this->w / (T)a);
    }
      
    inline vec4<T> operator /(const vec4<T> &vec) const
    {
      return vec4<T>(this->x / vec.x,
               this->y / vec.y,
               this->z / vec.z,
               this->w / vec.w);
    }
      
    inline const vec4<T> &operator =(const vec4<T> &vec)
    {
      this->x = vec.x;
      this->y = vec.y;
      this->z = vec.z;
      this->w = vec.w;
      return *this;
    }
      
    inline const vec4<T> operator +=(const vec4<T> &vec)
    {
      this->x += vec.x;
      this->y += vec.y;
      this->z += vec.z;
      this->w += vec.w;
      return *this;
    }
      
    inline const vec4<T> operator -=(const vec4<T> &vec)
    {
      this->x -= vec.x;
      this->y -= vec.y;
      this->z -= vec.z;
      this->w -= vec.w;
      return *this;
    }
      
    inline T &operator[](const int n)
    {
      return vals[n];
    }
      
    inline const T &operator[](const int n) const
    {
      return vals[n];
    }

    inline T GetLength() const
    {
      return sqrt(x * x + y * y + z * z + w * w);
    }

    inline vec4<T> Normalize() const
    {
      T len = this->GetLength();
      return vec4<T>(x / len, y / len, z / len, w / len);
    }

    inline operator vec3<T>() const
    {
      return vec3<T>(x, y, z);
    }
};
