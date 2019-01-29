
#include "Geometry.h"

//----------------
//--- Geometry ---
//----------------

real Geometry::Distance(const vec3r &point, const Geometry::Segment &segment)
{
	vec3r eb = segment.p2 - segment.p1;
	real proj = DotProduct(eb.Normalize(), (point - segment.p1));
	real ebl = eb.GetLength();
	if (proj >= 0 && proj <= ebl)
		return sqrt((point - segment.p1).GetLength2() - proj * proj);
	else if (proj >= 0)
		return (point - segment.p2).GetLength();
	else
		return (point - segment.p1).GetLength();
}

real Geometry::Distance(const Segment &segment1, const Segment &segment2)
{
  // Assumes, that they do not intersect and lay on one plane
  return std::min(Distance(segment1.p1, segment2),
                  std::min(Distance(segment1.p2, segment2),
                           std::min(Distance(segment2.p1, segment1),
                                    Distance(segment2.p2, segment1))));
}

real Geometry::ArcLength(const vec3r &center, real r, const vec3r &p1, const vec3r &p2)
{
	vec3r dp1 = p1 - center, dp2 = p2 - center;
    vec3r mid = dp1 + dp2;
	real mid_len = mid.GetLength();
    if (mid_len < r * (real)1e-3)
        return (real)(PI * r);
    mid = mid * (r / mid_len) + center;
    double res = (mid - p1).GetLength() * 2;
    res += (res - (p2 - p1).GetLength()) / 3.0;
    return (float)res;
}

bool Geometry::AreIntersected(const Geometry::Segment &seg, const Geometry::Plane &plane, vec3r &ipoint)
{
	//Step 1. Checking, that segment really intersects plane.
	real offset = DotProduct(plane.n, plane.p);
	real p1_proj = DotProduct(plane.n, seg.p1), p2_proj = DotProduct(plane.n, seg.p2);
	if ((p1_proj > offset && p2_proj > offset) || (p1_proj < offset && p2_proj < offset))
		return false;

	//Step 2. Getting intersection point between segment and plane.
	vec3r dir = seg.p2 - seg.p1;
	real div = DotProduct(dir, plane.n);
	if (div == (real)0.0) return false;		//in the same plane, ignoring
	real t = (offset - p1_proj) / div;
	ipoint = dir * t + seg.p1;
	
	return true;
}

bool Geometry::AreIntersected(const Geometry::Segment &seg, const Geometry::Rectangle &rect, vec3r &ipoint)
{
	//Getting intersection with rectangle-defined plane. 
	if (!AreIntersected(seg, Plane(rect.p, CrossProduct(rect.v1, rect.v2).Normalize()), ipoint))
		return false;

	//Checking that intersection point located inside rectangle.
	real v1_proj = DotProduct(rect.v1, ipoint - rect.p);
	real v2_proj = DotProduct(rect.v2, ipoint - rect.p);
	return v1_proj >= (real)0.0 && v1_proj <= rect.v1.GetLength2() &&
		   v2_proj >= (real)0.0 && v2_proj <= rect.v2.GetLength2();
}

bool Geometry::AreIntersected(const Geometry::Segment &seg, const Geometry::SemiCircle &circle, vec3r &ipoint)
{
	//Getting intersection with circle-defined plane. 
	if (!AreIntersected(seg, Plane(circle.p, CrossProduct(circle.r1, circle.r2).Normalize()), ipoint))
		return false;

	//Checking that intersection point located inside semicircle.
	vec3r dp = ipoint - circle.p;
	real r2_proj = DotProduct(circle.r2, dp);
	return r2_proj >= (real)0.0 && dp.GetLength2() <= circle.r1.GetLength2();
}

bool Geometry::AreIntersected(const Geometry::Segment &seg, const Geometry::SemiTube &tube, vec3r &ipoint)
{
	vec3r dir = (seg.p2 - seg.p1).Normalize();
	real len = (seg.p2 - seg.p1).GetLength();
	real r_sqr = tube.r.GetLength2();
	vec3r handDir = (tube.p2 - tube.p1).Normalize();
	// beg == seg.p1, hbeg == tube.p1
	// || beg + t*dir - hbeg - ((beg,hdir) + t*(dir,hdir) - (hbeg,hdir)) * hdir || = r
	// || beg - hbeg - (beg,hdir)*hdir + (hbeg,hdir)*hdir + t(dir - (dir,hdir)*hdir) || = r
	vec3r fi = seg.p1 - tube.p1 - handDir * DotProduct(seg.p1, handDir) + handDir * DotProduct(tube.p1, handDir);
	vec3r si = dir - handDir * DotProduct(dir, handDir);
	//  || fi + t * si || = cr_hand_r
	real a = DotProduct(si, si);
	real b = 2 * DotProduct(fi, si);
	real c = DotProduct(fi, fi) - r_sqr;
	// a*t^2 + b*t + c = 0
	real disc = b * b - 4 * a * c;
	if (disc >= (real)0)
	{
		real t[2] = { (-b - sqrt(disc)) / (2 * a), (-b + sqrt(disc)) / (2 * a) };
		bool cond[2] = { t[0] >= 0 && t[0] <= len, t[1] >= 0 && t[1] <= len };
		
		for (int i = 0 + (cond[0] ? 0 : 1); i < 2 - (cond[1] ? 0 : 1); i++)
		{
			// On the MT
			ipoint = seg.p1 + dir * t[i];
			if (DotProduct(ipoint - tube.p1, tube.r) > 0)
			{
				// Our semi-space
				if (DotProduct(ipoint - tube.p1, tube.p2 - tube.p1) >= 0 && DotProduct(ipoint - tube.p2, tube.p1 - tube.p2) >= 0)
				{
					// On our part of cylinder
					return true;
				}
			}
		}
	}
	return false;
}
