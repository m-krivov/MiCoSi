using System;
using SlimDX;
using SlimDX.Direct3D11;
using SlimDX.DXGI;
using SpriteTextRenderer;
using Device = SlimDX.Direct3D11.Device;
using SpriteRenderer = SpriteTextRenderer.SlimDX.SpriteRenderer;

namespace MiCoSi.DxRender
{
  internal abstract class AbstractDxRenderer : IDxRenderer
  {
    protected const float FoV = (float) Math.PI/4;

    protected readonly ConstantBuffer ConstBuffer;
    protected readonly DxDeviceContext DeviceContext;
    protected readonly DxMeshes Meshes;
    private DepthStencilView _depthStencilView;
    private RenderTargetView _renderTargetView;

    protected AbstractDxRenderer(DxDeviceContext deviceContext, Camera camera)
    {
      DeviceContext = deviceContext;
      Camera = camera;
      Meshes = new DxMeshes(deviceContext.Device);

      ConstBuffer = new ConstantBuffer(deviceContext.Device);
      ConstBuffer.World = Matrix.Identity;
      ConstBuffer.View = Matrix.Identity;
      ConstBuffer.Projection = Matrix.Identity;
      ConstBuffer.LightDirection = new Vector4(0.0f, 0.0f, 1.0f, 1.0f);
      ConstBuffer.CameraPosition = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    public Camera Camera { get; set; }

    public void Draw(DxRenderContext renderContext, Cell cell, SimParams parameters)
    {
      if (renderContext == null)
      {
        throw new ArgumentNullException("renderContext");
      }

      Viewport viewPort;
      renderContext.RenderTargetAspect.GetTargets(out _depthStencilView, out _renderTargetView, out viewPort);

      ConstBuffer.View = Matrix.Transpose(Camera.ViewMatrix);
      ConstBuffer.Projection = Matrix.Transpose(Matrix.PerspectiveFovLH(FoV,
                                                renderContext.RenderTargetAspect.AspectRatio,
                                                0.01f, 100.0f));
      ConstBuffer.LightDirection = new Vector4(Camera.Direction, 1.0f);
      //ConstBuffer.CameraPosition = new Vector4(Camera.Position, 1.0f); // WTF? Looks like bugged shader

      DeviceContext.Device.ImmediateContext.OutputMerger.SetTargets(_depthStencilView, _renderTargetView);
      DeviceContext.Device.ImmediateContext.Rasterizer.SetViewports(viewPort);

      ClearDepthStencil();
      ClearRenderTarget();

      DrawCore(renderContext, cell, parameters);

      renderContext.RenderTargetAspect.OnCompleteRender();

      _depthStencilView = null;
      _renderTargetView = null;
    }

    public abstract void Clear(DxRenderContext renderContext);

    public virtual void Dispose()
    {
      if (ConstBuffer != null)
      {
        ConstBuffer.Dispose();
      }

      if (Meshes != null)
      {
        Meshes.Dispose();
      }
    }

    protected abstract void DrawCore(DxRenderContext renderContext, Cell cell, SimParams parameters);

    protected void ClearDepthStencil()
    {
      if (_depthStencilView == null)
      {
        throw new NotSupportedException();
      }

      DeviceContext.Device.ImmediateContext.ClearDepthStencilView(_depthStencilView, DepthStencilClearFlags.Depth, 1.0f, 0);
    }

    protected void ClearRenderTarget()
    {
      if (_renderTargetView == null)
      {
        throw new NotSupportedException();
      }

      DeviceContext.Device.ImmediateContext.ClearRenderTargetView(_renderTargetView, new Color4(System.Drawing.Color.Black));
    }

    protected static void DrawMesh(Device device, DxMesh mesh)
    {
      device.ImmediateContext.DrawIndexed(mesh.IndexCount, 0, 0);
    }

    protected static void SetAndDrawMesh(Device device, DxMesh mesh)
    {
      SetMesh(device, mesh);
      DrawMesh(device, mesh);
    }

    protected static void SetBuffer(Device device, ConstantBuffer buffer)
    {
      device.ImmediateContext.VertexShader.SetConstantBuffer(buffer.Buffer, 0);
      device.ImmediateContext.PixelShader.SetConstantBuffer(buffer.Buffer, 0);
    }

    protected static void SetMesh(Device device, DxMesh mesh)
    {
      device.ImmediateContext.InputAssembler.SetVertexBuffers(0, new VertexBufferBinding(mesh.Vertices, 6 * sizeof(float), 0));
      device.ImmediateContext.InputAssembler.SetIndexBuffer(mesh.Indices, Format.R16_UInt, 0);
    }
  }
}
