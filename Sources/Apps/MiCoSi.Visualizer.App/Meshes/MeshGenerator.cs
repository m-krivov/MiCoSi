using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;

namespace MiCoSi
{
  /// <summary>
  /// Generates simple meshes
  /// </summary>
  internal static class MeshGenerator
  {
    private static SlimDX.Matrix RotationVectors(SlimDX.Vector3 v1, SlimDX.Vector3 v2)
    {
      v1.Normalize();
      v2.Normalize();
      Vector3 cp = Vector3.Cross(v1, v2);
      if (cp.Length() < 1e-3)
      {
        Vector3[] orts = { new Vector3(1, 0, 0), new Vector3(0, 1, 0), new Vector3(0, 0, 1) };
        foreach (Vector3 ort in orts)
        {
          cp = Vector3.Cross(v1, ort);
          if (cp.Length() > 1e-1)
            break;
        }
      }
      cp.Normalize();
      return Matrix.RotationAxis(cp, (float)Math.Acos(Vector3.Dot(v1, v2)));
    }

    /// <summary>
    /// Creates thin tube with required length and orientation.
    /// </summary>
    public static Mesh CreateTube(float length, float width, SlimDX.Vector3 direction)
    {
      float cos45 = (float)Math.Cos(Math.PI / 4);
      List<Vertex> vertices = new List<Vertex>();

      vertices.Add(new Vertex(-width / 2 * cos45, -width / 2 * cos45, 0,
                              -cos45, -cos45, 0));
      vertices.Add(new Vertex(width / 2 * cos45, -width / 2 * cos45, 0,
                             cos45, -cos45, 0));
      vertices.Add(new Vertex(0, width / 2, 0,
                              0, 1, 0));

      vertices.Add(new Vertex(-width / 2 * cos45, -width / 2 * cos45, length,
                              -cos45, -cos45, 0));
      vertices.Add(new Vertex(width / 2 * cos45, -width / 2 * cos45, length,
                              cos45, -cos45, 0));
      vertices.Add(new Vertex(0, width / 2, length,
                              0, 1, 0));

      vertices.Add(new Vertex(-width / 2 * cos45, -width / 2 * cos45, 0,
                              0, 0, -1));
      vertices.Add(new Vertex(width / 2 * cos45, -width / 2 * cos45, 0,
                              0, 0, -1));
      vertices.Add(new Vertex(0, width / 2, 0,
                              0, 0, -1));

      vertices.Add(new Vertex(-width / 2 * cos45, -width / 2 * cos45, length,
                              0, 0, 1));
      vertices.Add(new Vertex(width / 2 * cos45, -width / 2 * cos45, length,
                              0, 0, 1));
      vertices.Add(new Vertex(0, width / 2, length,
                              0, 0, 1));

      Matrix mat = RotationVectors(direction, new Vector3(0, 0, 1));
      foreach (var v in vertices)
        v.Transform(mat);

      List<IndexedPolygon> indices = new List<IndexedPolygon>();
      indices.Add(new IndexedPolygon(6, 8, 7));
      indices.Add(new IndexedPolygon(9, 10, 11));

      indices.Add(new IndexedPolygon(0, 3, 2));
      indices.Add(new IndexedPolygon(5, 2, 3));

      indices.Add(new IndexedPolygon(1, 2, 5));
      indices.Add(new IndexedPolygon(1, 5, 4));

      indices.Add(new IndexedPolygon(0, 1, 3));
      indices.Add(new IndexedPolygon(4, 3, 1));

      return new Mesh(vertices, indices);
    }

    private static void DetalizeAsSphere(List<Vertex> vertices,
                                        ref List<IndexedPolygon> indices,
                                        float radius,
                                        int stepCount)
    {
      for (int i = 0; i < stepCount; i++)
      {
        List<IndexedPolygon> newIndices = new List<IndexedPolygon>();
        foreach (var polygon in indices)
        {
          UInt16 v1 = polygon.P1;
          UInt16 v2 = polygon.P2;
          UInt16 v3 = polygon.P3;
          Vector3 pos;

          //Adding new vertices.
          UInt16 v4 = (UInt16)vertices.Count;
          pos = new Vector3((vertices[v1].X + vertices[v2].X) / 2,
                            (vertices[v1].Y + vertices[v2].Y) / 2,
                            (vertices[v1].Z + vertices[v2].Z) / 2);
          pos.Normalize();
          vertices.Add(new Vertex(pos.X * radius, pos.Y * radius, pos.Z * radius,
                                  pos.X, pos.Y, pos.Z));

          UInt16 v5 = (UInt16)vertices.Count;
          pos = new Vector3((vertices[v2].X + vertices[v3].X) / 2,
                            (vertices[v2].Y + vertices[v3].Y) / 2,
                            (vertices[v2].Z + vertices[v3].Z) / 2);
          pos.Normalize();
          vertices.Add(new Vertex(pos.X * radius, pos.Y * radius, pos.Z * radius,
                                  pos.X, pos.Y, pos.Z));

          UInt16 v6 = (UInt16)vertices.Count;
          pos = new Vector3((vertices[v1].X + vertices[v3].X) / 2,
                            (vertices[v1].Y + vertices[v3].Y) / 2,
                            (vertices[v1].Z + vertices[v3].Z) / 2);
          pos.Normalize();
          vertices.Add(new Vertex(pos.X * radius, pos.Y * radius, pos.Z * radius,
                                  pos.X, pos.Y, pos.Z));

          //Creating polygons.
          newIndices.Add(new IndexedPolygon(v1, v4, v6));
          newIndices.Add(new IndexedPolygon(v4, v2, v5));
          newIndices.Add(new IndexedPolygon(v4, v5, v6));
          newIndices.Add(new IndexedPolygon(v6, v5, v3));
        }

        indices.Clear();
        indices = newIndices;
      }
    }

    /// <summary>
    /// Creates geo-sphere.
    /// </summary>
    public static Mesh CreateSphere(float radius)
    {
      // Step 1. Building a cube
      float noffset = (float)(1 / Math.Sqrt(3));
      float offset = radius * noffset;
      List<Vertex> vertices = new List<Vertex>();

      vertices.Add(new Vertex(-offset, -offset, -offset,
                              -noffset, -noffset, -noffset));
      vertices.Add(new Vertex(-offset, offset, -offset,
                              -noffset, noffset, -noffset));
      vertices.Add(new Vertex(offset, offset, -offset,
                              noffset, noffset, -noffset));
      vertices.Add(new Vertex(offset, -offset, -offset,
                              noffset, -noffset, -noffset));

      vertices.Add(new Vertex(-offset, -offset, offset,
                              -noffset, -noffset, noffset));
      vertices.Add(new Vertex(-offset, offset, offset,
                              -noffset, noffset, noffset));
      vertices.Add(new Vertex(offset, offset, offset,
                              noffset, noffset, noffset));
      vertices.Add(new Vertex(offset, -offset, offset,
                              noffset, -noffset, noffset));

      List<IndexedPolygon> indices = new List<IndexedPolygon>();

      indices.Add(new IndexedPolygon(0, 1, 2));
      indices.Add(new IndexedPolygon(0, 2, 3));

      indices.Add(new IndexedPolygon(3, 2, 6));
      indices.Add(new IndexedPolygon(3, 6, 7));

      indices.Add(new IndexedPolygon(7, 6, 5));
      indices.Add(new IndexedPolygon(7, 5, 4));

      indices.Add(new IndexedPolygon(4, 5, 1));
      indices.Add(new IndexedPolygon(4, 1, 0));

      indices.Add(new IndexedPolygon(1, 6, 2));
      indices.Add(new IndexedPolygon(1, 5, 6));

      indices.Add(new IndexedPolygon(0, 3, 7));
      indices.Add(new IndexedPolygon(0, 7, 4));

      // Step 2.Transforming cube to a sphere
      DetalizeAsSphere(vertices, ref indices, radius, 6);

      return new Mesh(vertices, indices);
    }

    /// <summary>
    /// Creates and cuts sphere. The cut surface will be orthogonal to provided direction
    /// </summary>
    public static Mesh CreateEmptyHalfSphere(float radius, SlimDX.Vector3 direction)
    {
      //Step 1. Building a cube.
      float noffset1 = (float)(1 / Math.Sqrt(3));
      float voffset1 = radius * noffset1;
      float noffset2 = (float)(1 / Math.Sqrt(2));
      float voffset2 = radius * noffset2;
      List<Vertex> vertices = new List<Vertex>();

      vertices.Add(new Vertex(-voffset2, -voffset2, 0,
                              -noffset2, -noffset2, 0));
      vertices.Add(new Vertex(-voffset2, voffset2, 0,
                              -noffset2, noffset2, 0));
      vertices.Add(new Vertex(voffset2, voffset2, 0,
                              noffset2, noffset2, 0));
      vertices.Add(new Vertex(voffset2, -voffset2, 0,
                              noffset2, -noffset2, 0));

      vertices.Add(new Vertex(-voffset1, -voffset1, voffset1,
                              -noffset1, -noffset1, noffset1));
      vertices.Add(new Vertex(-voffset1, voffset1, voffset1,
                              -noffset1, noffset1, noffset1));
      vertices.Add(new Vertex(voffset1, voffset1, voffset1,
                              noffset1, noffset1, noffset1));
      vertices.Add(new Vertex(voffset1, -voffset1, voffset1,
                              noffset1, -noffset1, noffset1));

      List<IndexedPolygon> indices = new List<IndexedPolygon>();

      indices.Add(new IndexedPolygon(3, 2, 6));
      indices.Add(new IndexedPolygon(3, 6, 7));

      indices.Add(new IndexedPolygon(7, 6, 5));
      indices.Add(new IndexedPolygon(7, 5, 4));

      indices.Add(new IndexedPolygon(4, 5, 1));
      indices.Add(new IndexedPolygon(4, 1, 0));

      indices.Add(new IndexedPolygon(1, 6, 2));
      indices.Add(new IndexedPolygon(1, 5, 6));

      indices.Add(new IndexedPolygon(0, 3, 7));
      indices.Add(new IndexedPolygon(0, 7, 4));

      //Step 2. Transforming half-cube into a half-sphere.
      DetalizeAsSphere(vertices, ref indices, radius, 5);

      //Step 3. Adding inner polygons.
      int vCount = vertices.Count;
      for (int i = 0; i < vCount; i++)
      {
        Vertex v = vertices[i];
        vertices.Add(new Vertex(v.X, v.Y, v.Z,
                                -v.NX, -v.NY, -v.NZ));
      }

      int pCount = indices.Count;
      for (int i = 0; i < pCount; i++)
      {
        IndexedPolygon p = indices[i];
        indices.Add(new IndexedPolygon((UInt16)(p.P1 + vCount),
                                       (UInt16)(p.P3 + vCount),
                                       (UInt16)(p.P2 + vCount)));
      }

      //Step 4. Rotating result.
      Matrix mat = RotationVectors(direction, new Vector3(0, 0, 1));
      foreach (var v in vertices)
        v.Transform(mat);

      return new Mesh(vertices, indices);
    }

    /// <summary>
    /// Creates cylinder. Its axis will be parallel with privided direction.
    /// </summary>
    public static Mesh CreateHalfCylinder(float length, float radius, SlimDX.Vector3 direction, float angle)
    {
      int pointCount = 20;

      List<Vertex> vertices = new List<Vertex>();
      vertices.Add(new Vertex(0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, -1.0f)); 
      vertices.Add(new Vertex(0.0f, 0.0f, length,
                              0.0f, 0.0f, 1.0f));

      for (int i = 0; i < pointCount; i++)
      {
        float alpha = (float)(i * Math.PI / (pointCount - 1));
        float cosa = (float)Math.Cos(alpha);
        float sina = (float)Math.Sin(alpha);
        vertices.Add(new Vertex(cosa * radius, sina * radius, 0.0f,
                                0.0f, 0.0f, -1.0f));
        vertices.Add(new Vertex(cosa * radius, sina * radius, 0.0f,
                                cosa, sina, 0.0f));
        vertices.Add(new Vertex(cosa * radius, sina * radius, length,
                                0.0f, 0.0f, 1.0f));
        vertices.Add(new Vertex(cosa * radius, sina * radius, length,
                                cosa, sina, 0.0f));
      }
      vertices.Add(new Vertex(radius, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f));
      vertices.Add(new Vertex(-radius, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f));
      vertices.Add(new Vertex(radius, 0.0f, length,
                              0.0f, -1.0f, 0.0f));
      vertices.Add(new Vertex(-radius, 0.0f, length,
                              0.0f, -1.0f, 0.0f));

      List<IndexedPolygon> indices = new List<IndexedPolygon>();
      for (int i = 0; i < pointCount - 1; i++)
      {
        int cur = 4 * i + 2;
        int next = 4 * (i + 1) + 2;
        indices.Add(new IndexedPolygon((UInt16)0, (UInt16)next, (UInt16)cur));
        indices.Add(new IndexedPolygon((UInt16)(cur + 1), (UInt16)(next + 1), (UInt16)(cur + 3)));
        indices.Add(new IndexedPolygon((UInt16)(cur + 3), (UInt16)(next + 1), (UInt16)(next + 3)));
        indices.Add(new IndexedPolygon((UInt16)1, (UInt16)(cur + 2), (UInt16)(next + 2)));
      }
      int offset = 2 + pointCount * 4;
      indices.Add(new IndexedPolygon((UInt16)(offset + 0), (UInt16)(offset + 2), (UInt16)(offset + 1)));
      indices.Add(new IndexedPolygon((UInt16)(offset + 1), (UInt16)(offset + 2), (UInt16)(offset + 3)));

      Matrix mat = Matrix.RotationZ(angle);
      mat *= RotationVectors(direction, new Vector3(0, 0, 1));
      foreach (var v in vertices)
        v.Transform(mat);

      return new Mesh(vertices, indices);
    }

    public static Mesh CreateSpring(float springR, float springLength,
                                    SlimDX.Vector3 springDirection, int springDetalization,
                                    int loopCount, float wireR, int wireDetalization)
    {
      if (springDetalization < 3 || wireDetalization < 3)
        throw new ApplicationException("Too low detalization level");

      if (loopCount == 1)
        throw new ApplicationException("Cannot create spring without loops");

      float h = (float)(springLength / (2 * Math.PI * loopCount));
      List<Vertex> vertices = new List<Vertex>();
      List<IndexedPolygon> indices = new List<IndexedPolygon>();
      
      int[] prevStep = null;
      int[] curStep = null;

      for (int i = 0; i < loopCount; i++)
      {
        for (int j = 0; j < springDetalization; j++)
        {
          float alpha = (float)(2 * Math.PI * j / ((double)springDetalization - 1));
          float x = springR * (float)Math.Cos(alpha);
          float y = springR * (float)Math.Sin(alpha);
          float z = (float)(h * (alpha + 2 * Math.PI * i));

          SlimDX.Vector3 v1 = new Vector3(0, 0, wireR);
          SlimDX.Vector3 v2 = new Vector3(x, y, 0);
          v2.Normalize();
          v2 = v2 * wireR;

          curStep = new int[wireDetalization];
          for (int t = 0; t < wireDetalization; t++)
          {
            float beta = (float)(2 * Math.PI * t / (double)wireDetalization - 1);
            SlimDX.Vector3 v = v1 * (float)Math.Sin(beta) + v2 * (float)Math.Cos(beta);
            SlimDX.Vector3 nv = v;
            nv.Normalize();
            curStep[t] = vertices.Count;
            vertices.Add(new Vertex(x + v.X, y + v.Y, z + v.Z, nv.X, nv.Y, nv.Z));
          }

          if (prevStep != null)
          {
            for (int t = 0; t < wireDetalization; t++)
            {
              int tNext = (t + 1) % wireDetalization;
              indices.Add(new IndexedPolygon((UInt16)prevStep[t], (UInt16)curStep[t], (UInt16)prevStep[tNext]));
              indices.Add(new IndexedPolygon((UInt16)prevStep[tNext], (UInt16)curStep[t], (UInt16)curStep[tNext]));
            }
          }
          prevStep = curStep;
        }
      }

      SlimDX.Matrix mat = RotationVectors(springDirection, new Vector3(0, 0, 1));
      foreach (var v in vertices)
        v.Transform(mat);

      return new Mesh(vertices, indices);
    }
  }
}
