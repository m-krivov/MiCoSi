using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Mitosis
{
  public class Vec3
  {
    public double x { get; set; }
    public double y { get; set; }
    public double z { get; set; }

    public Vec3(double x, double y, double z) { this.x = x; this.y = y; this.z = z; }
    public Vec3() : this(0.0, 0.0, 0.0) { /*nothing*/ }
    public Vec3(Vec3D vec) : this(vec.X, vec.Y, vec.Z) { /*nothing*/ }

    public double Length
    { get { return Math.Sqrt(x * x + y * y + z * z); } }

    public Vec3 Normailzed
    {
      get
      {
        double len = Length;
        return new Vec3(x / len, y / len, z / len);
      }
    }

    public static double DotProduct(Vec3 v1, Vec3 v2)
    { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

    public static Vec3 CrossProduct(Vec3 v1, Vec3 v2)
    {
      return new Vec3(v1.y * v2.z - v1.z * v2.y,
                      v1.z * v2.x - v1.x * v2.z,
                      v1.x * v2.y - v1.y * v2.x);
    }

    public static Vec3 operator +(Vec3 vec1, Vec3 vec2) { return new Vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z); }
    public static Vec3 operator -(Vec3 vec1, Vec3 vec2) { return new Vec3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z); }
    public static Vec3 operator *(Vec3 vec1, Vec3 vec2) { return new Vec3(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z); }
    public static Vec3 operator *(Vec3 vec1, double val) { return new Vec3(vec1.x * val, vec1.y * val, vec1.z * val); }
    public static Vec3 operator /(Vec3 vec1, double val) { return new Vec3(vec1.x / val, vec1.y / val, vec1.z / val); }
    public static Vec3 operator -(Vec3 vec) { return new Vec3(-vec.x, -vec.y, -vec.z); }
  }
}
