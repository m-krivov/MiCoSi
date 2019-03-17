using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Mitosis
{
  public class Matrix3x3
  {
    private double[] _a = new double[9];

    public Matrix3x3()
    {
      _a[0] = 1.0;
      _a[4] = 1.0;
      _a[8] = 1.0;
    }

    public Matrix3x3(Mat3x3D mat)
    {
      for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
          _a[y * 3 + x] = mat[y, x];
    }

    public static Matrix3x3 operator *(Matrix3x3 mat1, Matrix3x3 mat2)
    {
      Matrix3x3 res = new Matrix3x3();

      res._a[0] = mat1._a[0] * mat2._a[0] +
                  mat1._a[1] * mat2._a[3] +
                  mat1._a[2] * mat2._a[6];
      res._a[1] = mat1._a[0] * mat2._a[1] +
                  mat1._a[1] * mat2._a[4] +
                  mat1._a[2] * mat2._a[7];
      res._a[2] = mat1._a[0] * mat2._a[2] +
                  mat1._a[1] * mat2._a[5] +
                  mat1._a[2] * mat2._a[8];
      res._a[3] = mat1._a[3] * mat2._a[0] +
                  mat1._a[4] * mat2._a[3] +
                  mat1._a[5] * mat2._a[6];
      res._a[4] = mat1._a[3] * mat2._a[1] +
                  mat1._a[4] * mat2._a[4] +
                  mat1._a[5] * mat2._a[7];
      res._a[5] = mat1._a[3] * mat2._a[2] +
                  mat1._a[4] * mat2._a[5] +
                  mat1._a[5] * mat2._a[8];
      res._a[6] = mat1._a[6] * mat2._a[0] +
                  mat1._a[7] * mat2._a[3] +
                  mat1._a[8] * mat2._a[6];
      res._a[7] = mat1._a[6] * mat2._a[1] +
                  mat1._a[7] * mat2._a[4] +
                  mat1._a[8] * mat2._a[7];
      res._a[8] = mat1._a[6] * mat2._a[2] +
                mat1._a[7] * mat2._a[5] +
                mat1._a[8] * mat2._a[8];

      return res;
    }

    public static Matrix3x3 RotationX(double angle)
    {
      Matrix3x3 res = new Matrix3x3();

      double cosa = Math.Cos(angle);
      double sina = Math.Sin(angle);
      res._a[0] = 1.0;
      res._a[4] = cosa;
      res._a[5] = -sina;
      res._a[7] = sina;
      res._a[8] = cosa;

      return res;
    }

    public static Matrix3x3 RotationY(double angle)
    {
      Matrix3x3 res = new Matrix3x3();

      double cosa = Math.Cos(angle);
      double sina = Math.Sin(angle);
      res._a[0] = cosa;
      res._a[2] = sina;
      res._a[4] = 1.0;
      res._a[6] = -sina;
      res._a[8] = cosa;

      return res;
    }

    public static Matrix3x3 RotationZ(double angle)
    {
      Matrix3x3 res = new Matrix3x3();

      double cosa = Math.Cos(angle);
      double sina = Math.Sin(angle);
      res._a[0] = cosa;
      res._a[1] = -sina;
      res._a[3] = sina;
      res._a[4] = cosa;
      res._a[8] = 1.0;

      return res;
    }

    public static Matrix3x3 RotationXYZ(double rot_x, double rot_y, double rot_z)
    {
      return RotationZ(rot_z) * RotationY(rot_y) * RotationX(rot_x);
    }

    public Vec3 Rotate(Vec3 vec)
    {
      return new Vec3(_a[0] * vec.x + _a[1] * vec.y + _a[2] * vec.z,
                      _a[3] * vec.x + _a[4] * vec.y + _a[5] * vec.z,
                      _a[6] * vec.x + _a[7] * vec.y + _a[8] * vec.z);
    }
  }
}
