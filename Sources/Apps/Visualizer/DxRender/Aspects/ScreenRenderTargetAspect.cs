using System;
using SlimDX;
using SlimDX.Direct3D11;
using SlimDX.DXGI;
using SpriteTextRenderer;
using Resource = SlimDX.Direct3D11.Resource;
using SpriteRenderer = SpriteTextRenderer.SlimDX.SpriteRenderer;

namespace Mitosis.DxRender
{
  internal class ScreenRenderTargetAspect : IRenderTargetAspect, IDisposable
  {
    private readonly DxDeviceContext _dxDeviceContext;
    private readonly RenderTargetView _renderTargetView;
    private readonly Texture2D _depthStencilTexture;
    private readonly DepthStencilView _depthStencilView;
    private readonly Viewport _viewport;
    private SpriteRenderer _spriteRenderer;
    private ShaderResourceView _redDotTexture;

    public ScreenRenderTargetAspect(DxDeviceContext dxDeviceContext)
    {
      _dxDeviceContext = dxDeviceContext;
      ShowRedDot = true;

      // Init render target from primary display
      Texture2D backBuffer = Resource.FromSwapChain<Texture2D>(_dxDeviceContext.SwapChain, 0);
      _renderTargetView = new RenderTargetView(_dxDeviceContext.Device, backBuffer);
      _dxDeviceContext.Device.ImmediateContext.ClearRenderTargetView(_renderTargetView, new Color4(System.Drawing.Color.Black));

      // Init depth stencil
      Texture2DDescription sdDesc = new Texture2DDescription();
      sdDesc.Width = _dxDeviceContext.FrameWidth;
      sdDesc.Height = _dxDeviceContext.FrameHeight;
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

      // Init viewport
      _viewport = new Viewport(0, 0, _dxDeviceContext.FrameWidth, _dxDeviceContext.FrameHeight, 0, 1);
    }

    public bool ShowRedDot { get; set; }

    public float AspectRatio
    {
      get
      { return (float)_dxDeviceContext.FrameWidth / _dxDeviceContext.FrameHeight; }
    }

    public void GetTargets(out DepthStencilView depthStencilView, out RenderTargetView renderTargetView, out Viewport viewport)
    {
      depthStencilView = _depthStencilView;
      renderTargetView = _renderTargetView;
      viewport = _viewport;
    }

    public void OnCompleteRender()
    {
      if (_spriteRenderer == null)
      {
        _spriteRenderer = new SpriteRenderer(_dxDeviceContext.Device, 128);
      }

      if (_redDotTexture == null)
      {
        String spritePath = System.IO.Path.Combine( Directories.GetResourceDirectory(), "LaserDot.png");

        if (!System.IO.File.Exists(spritePath))
          UserErrors.BadInstallation("resource", true);

        _redDotTexture
          = new ShaderResourceView(_dxDeviceContext.Device,
                       Texture2D.FromFile(_dxDeviceContext.Device, spritePath));
      }

      if (ShowRedDot)
      {
        _spriteRenderer.Draw(_redDotTexture, new Vector2(10, 10), new Vector2(10, 10), CoordinateType.Absolute);
        _spriteRenderer.Flush();
      }

      _dxDeviceContext.SwapChain.Present(0, PresentFlags.None);
    }

    public void Dispose()
    {
      if (_renderTargetView != null)
      {
        _renderTargetView.Dispose();
      }

      if (_depthStencilView != null)
      {
        _depthStencilView.Dispose();
      }

      if (_depthStencilTexture != null)
      {
        _depthStencilTexture.Dispose();
      }

      if (_redDotTexture != null)
      {
        _redDotTexture.Dispose();
      }

      if (_spriteRenderer != null)
      {
        _spriteRenderer.Dispose();
      }
    }
  }
}
