
#pragma once

#include "vec3x.h"

#define PI (3.14159265358979323846264338327950288419716939937510)

// Contains implementation for some geometry checks
class Geometry
{
  public:
    // Line segment defined by two points
    struct Segment
    {
      const vec3r p1, p2;
      Segment(const vec3r &_p1, const vec3r &_p2) : p1(_p1), p2(_p2) { /*nothing*/ }
    };

    // Plane defined by one anchor point and not normalized normal.
    struct Plane
    {
      const vec3r p, n;
      Plane(const vec3r &_p, const vec3r &_n) : p(_p), n(_n) { /*nothing*/ }
    };

    // Rectangle defined by one anchor point and two non-zero orthogonal vectors.
    struct Rectangle
    {
      const vec3r p, v1, v2;
      Rectangle(const vec3r &_p, const vec3r &_v1, const vec3r &_v2) : p(_p), v1(_v1), v2(_v2) { /*nothing*/ }
    };

    // Half of the filled circle defined by one point and two radiuses as (p-r1, p+r2, p+r1).
    // The lengthes of r1 and r2 must be equal.
    struct SemiCircle
    {
      const vec3r p, r1, r2;
      SemiCircle(const vec3r &_p, const vec3r &_r1, const vec3r &_r2) : p(_p), r1(_r1), r2(_r2) { /*nothing*/ }
    };

    // Half of the empty cyllinder defined by two axis point (p1, p2) and one radius (r).
    struct SemiTube
    {
      const vec3r p1, p2, r;
      SemiTube(const vec3r &_p1, const vec3r &_p2, const vec3r &_r) : p1(_p1), p2(_p2), r(_r) { /*nothing*/ }
    };

    Geometry(real eps = std::numeric_limits<real>::min());
    Geometry(const Geometry &) = delete;
    void operator =(const Geometry &) = delete;

    // Returns the epsilon: some threshold that defines that a number may be replaced by zero
    real GetEpsilon() const { return _eps; }

    // Sets new epsilon (must be a positive number)
    void SetEpsilon(real eps);

    // Returns distance from p-point to line segment defined as [s1, s2]
    real Distance(const vec3r &point, const Segment &segment) const;

    // Returns the minimal distance between two line segments
    real Distance(const Segment &segment1, const Segment &segment2) const;

    // Returns length of the shortest arc from p1 to p2 point
    // Expects that points is located on the circle (but renormàlizes them, for sure)
    real ArcLength(const vec3r &center, real r, const vec3r &p1, const vec3r &p2) const;

    // Returns true and sets intersection point in case of collision. False otherwise.
    bool AreIntersected(const Segment &seg, const Plane &plane, vec3r &ipoint) const;

    // Returns true and sets intersection point in case of collision. False otherwise.
    bool AreIntersected(const Segment &seg, const Rectangle &rect, vec3r &ipoint) const;

    // Returns true and sets intersection point in case of collision. False otherwise.
    bool AreIntersected(const Segment &seg, const SemiCircle &circle, vec3r &ipoint) const;

    // Returns true and sets intersection point in case of collision. False otherwise.
    bool AreIntersected(const Segment &seg, const SemiTube &tube, vec3r &ipoint) const;

  private:
    real _eps;
};
