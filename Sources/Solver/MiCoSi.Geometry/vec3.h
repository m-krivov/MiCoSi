#pragma once

template <class T> class vec3;
template <class T> class vec4;

template <class T>
static inline T DotProduct(const vec3<T> &a, const vec3<T> &b)
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

template <class T>
static inline vec3<T> CrossProduct(const vec3<T> &a, const vec3<T> &b)
{
  return vec3<T>(a.y * b.z - a.z * b.y,
                   -(a.x * b.z - a.z * b.x),
                   a.x * b.y - a.y * b.x);
}

template <class T>
class vec3
{
  private:
    T vals[3];

  public:
    T &x;
    T &y;
    T &z;

    // Constructors

    inline vec3<T>(): x(vals[0]), y(vals[1]), z(vals[2])
    {
      x = (T)0.0;
      y = (T)0.0;
      z = (T)0.0;
    }

    inline vec3<T>(const vec3<T> &vec): x(vals[0]), y(vals[1]), z(vals[2])
    {
      x = vec.x;
      y = vec.y;
      z = vec.z;
    }

    inline vec3<T>(const vec4<T> &vec): x(vals[0]), y(vals[1]), z(vals[2])
    {
      x = vec.x;
      y = vec.y;
      z = vec.z;
    }
      
    inline vec3<T>(T x, T y, T z): x(vals[0]), y(vals[1]), z(vals[2])
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }

    // Operators
      
    inline vec3<T> operator -() const
    {
      return vec3<T>(-x, -y, -z);
    }
      
    inline vec3<T> operator -(const vec3<T> &vec) const
    {
      return vec3(this->x - vec.x,
            this->y - vec.y,
            this->z - vec.z);
    }
      
    inline vec3<T> operator +(const vec3<T> &vec) const
    {
      return vec3<T>(this->x + vec.x,
               this->y + vec.y,
               this->z + vec.z);
    }
      
    template <class T2>
    inline vec3<T> operator *(T2 a) const
    {
      return vec3<T>(this->x * (T)a,
               this->y * (T)a,
               this->z * (T)a);
    }
      
    inline vec3<T> operator *(const vec3<T> &vec) const
    {
      return vec3<T>(this->x * vec.x,
               this->y * vec.y,
               this->z * vec.z);
    }
      
    template <class T2>
    inline vec3<T> operator /(T2 a) const
    {
      return vec3<T>(this->x / (T)a,
               this->y / (T)a,
               this->z / (T)a);
    }
      
    inline vec3<T> operator /(const vec3<T> &vec) const
    {
      return vec3<T>(this->x/vec.x,
               this->y/vec.y,
               this->z/vec.z);
    }
      
    inline const vec3<T> &operator =(const vec3<T> &vec)
    {
      this->x = vec.x;
      this->y = vec.y;
      this->z = vec.z;
      return *this;
    }
      
    inline const vec3<T> operator +=(const vec3<T> &vec)
    {
      this->x += vec.x;
      this->y += vec.y;
      this->z += vec.z;
      return *this;
    }
      
    inline const vec3<T> operator -=(const vec3<T> &vec)
    {
      this->x -= vec.x;
      this->y -= vec.y;
      this->z -= vec.z;
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

    inline bool operator ==(const vec3<T> &vec)
    { return x == vec.x && y == vec.y && z == vec.z; }

    inline bool operator !=(const vec3<T> &vec)
    { return !(*this == vec); }

    inline T GetLength() const
    {
      return sqrt(x * x + y * y + z * z);
    }

    inline T GetLength2() const
    {
      return x * x + y * y + z * z;
    }

    inline vec3<T> Normalize() const
    {
      T len = this->GetLength();
      return vec3<T>(x / len, y / len, z / len);
    }

    inline operator vec4<T>() const
    {
      return vec4<T>(x, y, z, 1);
    }
};

// Helper that allows to assing vec3<T> to interleaved vec3<T>-based array.
template <class T>
class vec3_assigner
{
  private:
    T *_x, *_y, *_z;

  public:
    T &x, &y, &z;
    inline vec3_assigner(T *px, T *py, T *pz)
      : _x(px), _y(py), _z(pz),
        x(*px), y(*py), z(*pz)
    { /*nothing*/ }

    inline const vec3<T> &operator=(const vec3<T> &vec)
    {
      *_x = vec.x;
      *_y = vec.y;
      *_z = vec.z;
      
      return vec;
    }

    inline operator vec3<T>()
    { return vec3<T>(*_x, *_y, *_z); }
};
