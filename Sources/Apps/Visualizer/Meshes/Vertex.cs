using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  /// <summary>
  /// Represents vertex as three coordinates and one normal
  /// </summary>
  internal class Vertex
  {
    public float X;
    public float Y;
    public float Z;

    public float NX;
    public float NY;
    public float NZ;

    public Vertex(float x, float y, float z,
            float nx, float ny, float nz)
    {
      X = x;
      Y = y;
      Z = z;

      NX = nx;
      NY = ny;
      NZ = nz;
    }

    public void Transform(SlimDX.Matrix rotation)
    {
      SlimDX.Vector4 coords = SlimDX.Vector4.Transform(new SlimDX.Vector4(X, Y, Z, 1.0f), rotation);
      SlimDX.Vector4 normals = SlimDX.Vector4.Transform(new SlimDX.Vector4(NX, NY, NZ, 1.0f), rotation);

      X = coords.X;
      Y = coords.Y;
      Z = coords.Z;

      NX = normals.X;
      NY = normals.Y;
      NZ = normals.Z;

      if (Math.Abs((float)Math.Sqrt(NX * NX + NY * NY + NZ * NZ) - 1.0f) > 1e-3)
        throw new ApplicationException(
                "Internal error: rotation matrix also performs translation and/or scaling");
    }

    public void WriteTo(SlimDX.DataStream stream)
    {
      stream.Write(X);
      stream.Write(Y);
      stream.Write(Z);

      stream.Write(NX);
      stream.Write(NY);
      stream.Write(NZ);
    }
  }
}
