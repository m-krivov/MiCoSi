
#pragma once

#include "Mitosis.Geometry/mat3x3x.h"

namespace Mitosis
{
  ///<summary>
  /// Wraps mat3x3r into managed structure. Always provides numbers with double precision.
  ///</summary>
  public ref struct Mat3x3D
  {
    private:
      const mat3x3r *_mat;
      mat3x3r *_local;

    private:
      void SafeRelease()
      {
        if (_local != NULL)
          delete _local;
        _local = NULL;
        _mat = NULL;
      }

    internal:
      Mat3x3D(const mat3x3r *mat)
      { _mat = mat; _local = NULL; }

      Mat3x3D(const mat3x3r &mat)
      { _local = new mat3x3r(mat); _mat = _local; }

      const mat3x3r *GetObject()
      { return _mat; }

    public:
      property double default[int, int]
      {
        double get(int row, int col)
        { return _mat->a[row * 3 + col]; }
      }

      !Mat3x3D()
      { SafeRelease(); }

      ~Mat3x3D()
      { SafeRelease(); }
  };
}
