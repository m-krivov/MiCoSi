using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D11;
using SlimDX.DXGI;

namespace MiCoSi.DxRender
{
  internal class TextureRenderTargetAspect : IRenderTargetAspect, IDisposable
  {
    private DxDeviceContext _dxDeviceContext;
    private int _width;
    private int _height;

    private Texture2D _renderTexture;
    private Texture2D _lockableTexture;
    private RenderTargetView _renderTargetView;
    private Texture2D _depthStencilTexture;
    private DepthStencilView _depthStencilView;
    private Viewport _viewport;

    public TextureRenderTargetAspect(DxDeviceContext dxDeviceContext, int width, int height)
    {
      _dxDeviceContext = dxDeviceContext;
      _width = width;
      _height = height;

      Texture2DDescription rtDesc = new Texture2DDescription();
      rtDesc.Width = _width;
      rtDesc.Height = _height;
      rtDesc.MipLevels = 1;
      rtDesc.ArraySize = 1;
      rtDesc.Format = Format.B8G8R8A8_UNorm;
      rtDesc.SampleDescription = new SlimDX.DXGI.SampleDescription(1, 0);
      rtDesc.Usage = ResourceUsage.Default;
      rtDesc.BindFlags = BindFlags.RenderTarget;
      rtDesc.CpuAccessFlags = CpuAccessFlags.None;
      rtDesc.OptionFlags = ResourceOptionFlags.None;
      _renderTexture = new Texture2D(dxDeviceContext.Device, rtDesc);
      _renderTargetView = new RenderTargetView(_dxDeviceContext.Device, _renderTexture);
      _dxDeviceContext.Device.ImmediateContext.ClearRenderTargetView(_renderTargetView, new Color4(System.Drawing.Color.Black));

      rtDesc.Usage = ResourceUsage.Staging;
      rtDesc.BindFlags = BindFlags.None;
      rtDesc.CpuAccessFlags = CpuAccessFlags.Read;
      _lockableTexture = new Texture2D(_dxDeviceContext.Device, rtDesc);

      Texture2DDescription sdDesc = new Texture2DDescription();
      sdDesc.Width = _width;
      sdDesc.Height = _height;
      sdDesc.MipLevels = 1;
      sdDesc.ArraySize = 1;
      sdDesc.Format = Format.D24_UNorm_S8_UInt;
      sdDesc.SampleDescription = new SampleDescription(1, 0);
      sdDesc.Usage = ResourceUsage.Default;
      sdDesc.BindFlags = BindFlags.DepthStencil;
      sdDesc.CpuAccessFlags = CpuAccessFlags.None;
      sdDesc.OptionFlags = ResourceOptionFlags.None;
      _depthStencilTexture = new Texture2D(_dxDeviceContext.Device, sdDesc);
      DepthStencilViewDescription dsvDesc = new DepthStencilViewDescription();
      dsvDesc.Format = sdDesc.Format;
      dsvDesc.Dimension = DepthStencilViewDimension.Texture2D;
      dsvDesc.FirstArraySlice = 0;
      dsvDesc.ArraySize = 1;
      _depthStencilView = new DepthStencilView(_dxDeviceContext.Device, _depthStencilTexture, dsvDesc);

      _viewport = new Viewport(0, 0, _width, _height, 0, 1);
    }

    public float AspectRatio
    {
      get
      { return (float)_width / _height; }
    }

    public void GetTargets(out SlimDX.Direct3D11.DepthStencilView depthStencilView, out SlimDX.Direct3D11.RenderTargetView renderTargetView, out SlimDX.Direct3D11.Viewport viewport)
    {
      depthStencilView = _depthStencilView;
      renderTargetView = _renderTargetView;
      viewport = _viewport;
    }

    public void OnCompleteRender()
    { /*nothing*/ }

    public void Lock(out IntPtr data, out int width, out int height, out int pitchInBytes)
    {
      _dxDeviceContext.Device.ImmediateContext.CopyResource(_renderTexture, _lockableTexture);

      DataBox dBox
        = _dxDeviceContext.Device.ImmediateContext.MapSubresource(_lockableTexture,
                                                                  0, 0,
                                                                  MapMode.Read,
                                                                  SlimDX.Direct3D11.MapFlags.None);
      data = dBox.Data.DataPointer;
      width = _width;
      height = _height;
      pitchInBytes = dBox.RowPitch;
    }

    public void UnLock()
    {
      _dxDeviceContext.Device.ImmediateContext.UnmapSubresource(_lockableTexture, 0);
    }

    private void SafeRelease()
    {
      if (_lockableTexture != null)
        _lockableTexture.Dispose();
      _lockableTexture = null;

      if (_depthStencilView != null)
        _depthStencilView.Dispose();
      _depthStencilView = null;

      if (_depthStencilTexture != null)
        _depthStencilTexture.Dispose();
      _depthStencilTexture = null;

      if (_renderTargetView != null)
        _renderTargetView.Dispose();
      _renderTargetView = null;

      if (_renderTexture != null)
        _renderTexture.Dispose();
      _renderTexture = null;
    }

    public void Dispose()
    { SafeRelease(); }

    ~TextureRenderTargetAspect()
    { SafeRelease(); }
  }
}
