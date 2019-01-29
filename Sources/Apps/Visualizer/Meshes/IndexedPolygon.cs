using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  /// <summary>
  /// Just a three integer integer that describes one polygon
  /// </summary>
  internal struct IndexedPolygon
  {
    public UInt16 P1;
    public UInt16 P2;
    public UInt16 P3;

    public IndexedPolygon(UInt16 p1, UInt16 p2, UInt16 p3)
    {
      P1 = p1;
      P2 = p2;
      P3 = p3;
    }
  }
}
