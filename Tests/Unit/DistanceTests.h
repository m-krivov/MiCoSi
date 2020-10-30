#pragma once
#include "Defs.h"

#include "Mitosis.Geometry/Geometry.h"

TEST(Distance, PointToSegment1)
{
  Geometry geom(1e-5f);
  auto s1 = vec3r(-1.0f, 0.0f, 0.0f);
  auto s2 = vec3r(1.0f, 0.0f, 0.0f);
  auto p = vec3r(0.0f, 1.0f, 0.0f);
  ASSERT_EQ_EPS(geom.Distance(p, Geometry::Segment(s1, s2)), 1.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(p, Geometry::Segment(s2, s1)), 1.0f, 1e-3f);
}

TEST(Distance, PointToSegment2)
{
  Geometry geom(1e-5f);
  auto s1 = vec3r(1.0f, 0.0f, 1.0f);
  auto s2 = vec3r(-1.0f, 0.0f, -1.0f);
  Geometry::Segment seg(s1, s2);
  ASSERT_EQ_EPS(geom.Distance(vec3r(2.0f, 0.0f, 2.0f), seg), std::sqrtf(2.0f), 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(vec3r(0.0f, 0.0f, 0.0f), seg), 0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(vec3r(-3.0f, -3.0f, 0.0f), seg), std::sqrtf(14.0f), 1e-3f);
}

TEST(Distance, PointToSegment3)
{
  Geometry geom(1e-5f);
  const size_t POINTS = 30u;
  const float R = 10.0f;
  Geometry::Segment seg(vec3r(0.0f, 0.0f, -10.0f), vec3r(0.0f, 0.0f, 10.0f));
  for (size_t i = 0; i < POINTS; i++)
  {
    float t = i / (float)(POINTS - 1);
    vec3r p(std::sinf(t * 3 * 2 * (real)PI) * R,
            std::cosf(t * 3 * 2 * (real)PI) * R,
            -5.0f + 10.0f * t);
    ASSERT_EQ_EPS(geom.Distance(p, seg), R, 1e-3f);
  }
}

TEST(Distance, PointToSegment4)
{
  Geometry geom(1e-5f);
  Geometry::Segment seg(vec3r(1.0f, 1.0f, 1.0f), vec3r(1.0f, 1.0f, 1.0f));
  ASSERT_EQ_EPS(geom.Distance(vec3r(1.0f, 1.0f, 1.0f), seg), 0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(vec3r(0.0f, 0.0f, 0.0f), seg), std::sqrt(3.0f), 1e-3f);
}

TEST(Distance, SegmentToSegment1)
{
  Geometry geom(1e-5f);
  vec3r p11( 1.0f, 1.0f, 0.0f), p12( 1.0f, -1.0f, 0.0f);
  vec3r p21(-1.0f, 1.0f, 0.0f), p22(-1.0f, -1.0f, 0.0f);
  vec3r pc1( 0.0f, 0.0f, 1.0f), pc2( 0.0f,  0.0f, -1.0f);

  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(p11, p12), Geometry::Segment(p12, p22)),
                0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(p11, p12), Geometry::Segment(p21, p22)),
                2.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(p11, p22), Geometry::Segment(pc1, pc2)),
                0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(pc1, pc2), Geometry::Segment(p21, p22)),
                1.0f, 1e-3f);
}

TEST(Distance, SegmentToSegment2)
{
  Geometry geom(1e-5f);
  vec3r p0(0.0f, 0.0f, 0.0f), p1(-1.0f, 1.0f, -1.0f), p2(1.0f, -1.0f, 1.0f);

  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(p1, p1), Geometry::Segment(p1, p1)), 0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(p1, p1), Geometry::Segment(p2, p2)),
                              std::sqrtf(12.0f), 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(p0, p1), Geometry::Segment(p2, p2)),
                              std::sqrtf(3.0f), 1e-3f);
}

TEST(Distance, SegmentToSegment3)
{
  Geometry geom(1e-5f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(vec3r(0.0f, 0.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                              Geometry::Segment(vec3r(0.0f, 1.0f, 0.0f), vec3r(1.0f, 1.0f, 0.0f))),
                1.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(vec3r(0.0f, 0.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                              Geometry::Segment(vec3r(1.0f, 1.0f, 0.0f), vec3r(1.0f, 2.0f, 0.0f))),
                1.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(vec3r(0.0f, 0.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                              Geometry::Segment(vec3r(1.0f, 0.0f, 0.0f), vec3r(2.0f, 0.0f, 0.0f))),
                0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.Distance(Geometry::Segment(vec3r(-1.0f, 0.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                              Geometry::Segment(vec3r(0.0f, 1.0f, 0.0f),  vec3r(0.0f, -1.0f, 0.0f))),
                0.0f, 1e-3f);
}

TEST(Distance, ArcLength_Correct)
{
  Geometry geom(1e-5f);
  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 1.0f,
                               vec3r(-1.0f, 0.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                (real)PI, 1e-3f);
  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 2.0f, 0.0f), 2.0f,
                               vec3r(-2.0f, 2.0f, 0.0f), vec3r(2.0f, 2.0f, 0.0f)),
                (real)PI * 2.0f, 1e-3f);

  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 100.0f,
                               vec3r(0.0f, 100.0f, 0.0f), vec3r(0.0f, 100.0f, 0.0f)),
                0.0f, 1e-3f);
  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 0.0f,
                               vec3r(0.0f, 1.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                0.0f, 1e-3f);


  real c_45 = 1 / std::sqrtf(2.0f);
  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 1.0f,
                               vec3r(-c_45, c_45, 0.0f), vec3r(c_45, c_45, 0.0f)),
                (real)PI / 2, 1e-1f);
  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 1.0f,
                               vec3r(c_45, c_45, 0.0f), vec3r(-c_45, c_45, 0.0f)),
                (real)PI / 2, 1e-1f);
}

TEST(Distance, ArcLength_Wrong)
{
  Geometry geom(1e-5f);
  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), -1.0f,
                               vec3r(0.0f, 1.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                0.0f, 1e-3f);

  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 0.5f,
                               vec3r(-1.0f, 0.0f, 0.0f), vec3r(1.0f, 0.0f, 0.0f)),
                (real)PI * 0.5f, 1e-3f);

  ASSERT_EQ_EPS(geom.ArcLength(vec3r(0.0f, 0.0f, 0.0f), 1.0f,
                               vec3r(-0.5f, 0.0f, 0.0f), vec3r(0.75f, 0.0f, 0.0f)),
                (real)PI, 1e-3f);
}
