using System;
using System.Collections.Generic;
using System.Text;
using SlimDX.Direct3D11;

namespace MiCoSi
{

  internal class DxMesh
  {
    public SlimDX.Direct3D11.Buffer Vertices { get; private set; }
    public int VertexCount { get; private set; }
    public SlimDX.Direct3D11.Buffer Indices { get; private set; }
    public int IndexCount { get; private set; }

    public DxMesh(SlimDX.Direct3D11.Buffer vertices, int vertexCount,
                  SlimDX.Direct3D11.Buffer indices, int indexCount)
    {
      Vertices = vertices;
      VertexCount = vertexCount;
      Indices = indices;
      IndexCount = indexCount;
    }
  }

  /// <summary>
  /// Auxiliary class that helps to create and manage meshes.
  /// </summary>
  internal class DxMeshes : IDisposable
  {
    private Device _device;
    private Dictionary<String, DxMesh> _meshes;

    /// <summary>
    /// Converts mesh to DX-buffers.
    /// </summary>
    private static void MeshToBuffers(Device device,
                                      Mesh mesh,
                                      out SlimDX.Direct3D11.Buffer vertices,
                                      out SlimDX.Direct3D11.Buffer indices)
    {
      SlimDX.DataStream dsVertices = new SlimDX.DataStream(mesh.Vertices.Length * 6 * sizeof(float), true, true);
      SlimDX.DataStream dsIndices = new SlimDX.DataStream(mesh.Indices.Length * 3 * sizeof(int), true, true);
      vertices = null;
      indices = null;

      try
      {
        foreach (Vertex v in mesh.Vertices)
          v.WriteTo(dsVertices);
        dsVertices.Position = 0;
        BufferDescription bdVertices = new BufferDescription();
        bdVertices.BindFlags = BindFlags.VertexBuffer;
        bdVertices.CpuAccessFlags = CpuAccessFlags.None;
        bdVertices.OptionFlags = ResourceOptionFlags.None;
        bdVertices.SizeInBytes = mesh.Vertices.Length * 6 * sizeof(float);
        bdVertices.Usage = ResourceUsage.Default;
        vertices = new SlimDX.Direct3D11.Buffer(device, dsVertices, bdVertices);

        dsIndices.WriteRange(mesh.Indices);
        dsIndices.Position = 0;
        BufferDescription bdIndices = new BufferDescription();
        bdIndices.BindFlags = BindFlags.IndexBuffer;
        bdIndices.CpuAccessFlags = CpuAccessFlags.None;
        bdIndices.OptionFlags = ResourceOptionFlags.None;
        bdIndices.SizeInBytes = mesh.Indices.Length * 3 * sizeof(int);
        bdIndices.Usage = ResourceUsage.Default;
        indices = new SlimDX.Direct3D11.Buffer(device, dsIndices, bdIndices);
      }
      catch (Exception)
      {
        if (vertices != null)
        {
          vertices.Dispose();
          vertices = null;
        }
        if (indices != null)
        {
          indices.Dispose();
          indices = null;
        }
        throw;
      }
      finally
      {
        dsVertices.Dispose();
        dsIndices.Dispose();
      }
    }

    public DxMeshes(Device device)
    {
      _device = device;
      _meshes = new Dictionary<string, DxMesh>();
    }

    /// <summary>
    /// Registers mesh with provided name. It allows to get corresponding DX-buffers.
    /// </summary>
    public void Register(String identifier, Mesh mesh)
    {
      SlimDX.Direct3D11.Buffer vertices;
      SlimDX.Direct3D11.Buffer indices;
      MeshToBuffers(_device, mesh, out vertices, out indices);
      _meshes[identifier] = new DxMesh(vertices, mesh.Vertices.Length, indices, mesh.Indices.Length * 3);
    }

    /// <summary>
    /// Returns DX-buffers that was created by "Register()" call.
    /// </summary>
    public DxMesh GetMesh(String identifier)
    {
      DxMesh value;
      if (!_meshes.TryGetValue(identifier, out value))
        throw new ApplicationException("Internal error: mesh with required name was not found");
      return value;
    }

    public void Dispose()
    {
      Exception err = null;

      foreach (var mesh in _meshes)
      {
        try
        {
          if (mesh.Value.Vertices != null)
            mesh.Value.Vertices.Dispose();
          if (mesh.Value.Indices != null)
            mesh.Value.Indices.Dispose();
        }
        catch (Exception ex)
        { err = ex; }
      }

      _meshes.Clear();
      if (err != null)
        throw err;
    }
  }
}
