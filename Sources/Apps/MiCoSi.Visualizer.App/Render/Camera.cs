using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;

namespace MiCoSi
{
  internal class Camera : ICloneable
  {
    public Vector3 Position { get; set; }
    public Vector3 Target { get; set; }
    public Vector3 Up { get; set; }

    public Camera(Vector3 position, Vector3 target, Vector3 up)
    {
      Position = position;
      Target = target;
      Up = up;
    }

    public Camera()
      : this(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f))
    { }

    public Vector3 Direction
    {
      get
      {
        Vector3 res = Target - Position;
        if (res.Length() < 1e-3)
          res = new Vector3(0.0f, 0.0f, 1.0f);
        else
          res.Normalize();
        return res;
      }
    }

    public Matrix ViewMatrix
    {
      get
      {
        if (Up.Length() < 1e-3)
          Up = new Vector3(0.0f, 1.0f, 0.0f);
        return Matrix.LookAtLH(Position, Target, Up);
      }
    }

    public Object Clone()
    { return new Camera(Position, Target, Up); }
  }
}
