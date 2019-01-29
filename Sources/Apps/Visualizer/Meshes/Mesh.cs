using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  /// <summary>
  /// Contains mesh as set of vertices and normals
  /// Uses index buffer.
  /// </summary>
  internal class Mesh
  {
    public Vertex[] Vertices { get; private set; }
    public IndexedPolygon[] Indices { get; private set; }

    public Mesh(IEnumerable<Vertex> vertices, IEnumerable<IndexedPolygon> indices)
    {
      Vertices = new List<Vertex>(vertices).ToArray();
      Indices = new List<IndexedPolygon>(indices).ToArray();
    }
  }
}
