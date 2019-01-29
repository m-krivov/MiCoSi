
#pragma once

#include "Mitosis.Geometry/vec3x.h"

namespace Mitosis
{
  ///<summary>
  /// Wraps vec3r into managed structure. Always provides numbers with double precision.
  ///</summary>
  public ref struct Vec3D
  {
    private:
      const vec3r *_vec;
      vec3r *_local;

    private:
      void SafeRelease()
      {
        if (_local != NULL)
          delete _local;
        _local = NULL;
        _vec = NULL;
      }

    internal:
      Vec3D(const vec3r *vec)
      { _vec = vec; _local = NULL; }

      Vec3D(const vec3r &vec)
      { _local = new vec3r(vec); _vec = _local; }

      const vec3r *GetObject()
      { return _vec; }

    public:
      property double X
      {
        double get()
        { return _vec->x; }
      }

      property double Y
      {
        double get()
        { return _vec->y; }
      }

      property double Z
      {
        double get()
        { return _vec->z; }
      }

      !Vec3D()
      { SafeRelease(); }

      ~Vec3D()
      { SafeRelease(); }
  };
}
