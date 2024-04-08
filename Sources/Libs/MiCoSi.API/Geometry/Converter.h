#pragma once

#include "Vector3.h"

namespace MiCoSi
{

// Helper that casts native vectors and matrices to C# analogues and vice versa
private ref class Converter abstract sealed
{
  public:
    static Vector3 ToVector3(const vec3r &vec)
    {
      return Vector3((float)vec.x, (float)vec.y, (float)vec.z);
    }

    static vec3r ToVec3R(Vector3 %vec)
    {
      return vec3r((real)vec.X, (real)vec.Y, (real)vec.Z);
    }
};

} // namespace MiCoSi
