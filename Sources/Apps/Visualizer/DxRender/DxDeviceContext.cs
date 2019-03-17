using System;
using System.Windows.Forms;
using SlimDX.D3DCompiler;
using SlimDX.Direct3D11;
using SlimDX.DirectInput;
using SlimDX.DXGI;
using Device = SlimDX.Direct3D11.Device;
using EffectFlags = SlimDX.D3DCompiler.EffectFlags;

namespace Mitosis.DxRender
{
  /// <summary>
  /// DxDeviceContext is a DX-specific object container that can be safely shared across different DxRender instances
  /// </summary>
  internal class DxDeviceContext : IDisposable
  {
    private static String GetFxFile()
    {
      String filename = System.IO.Path.Combine(Directories.GetResourceDirectory(), "Shaders.fx");
      if (System.IO.File.Exists(filename))
        return filename;

      throw new ApplicationException("Cannot find file with effects \"Shaders.fx\". Try to reinstall application");
    }

    private bool _isFullscreen;

    private DirectInput _dInput;
    private Mouse _mouse;

    public DxDeviceContext(IntPtr formHandle, Control renderAreaObject, bool fullscreen)
    {
      _isFullscreen = fullscreen;

      SwapChainDescription desc = new SwapChainDescription();

      if (fullscreen)
      {
        throw new ApplicationException("Not implemented yet");
      }
      else
      {
        FrameWidth = renderAreaObject.Width;
        FrameHeight = renderAreaObject.Height;

        desc.BufferCount = 1;
        desc.ModeDescription = new ModeDescription(FrameWidth,
                                                   FrameHeight,
                                                   new SlimDX.Rational(60, 1),
                                                   Format.R8G8B8A8_UNorm);
        desc.IsWindowed = true;
        desc.SwapEffect = SwapEffect.Discard;
      }

      desc.OutputHandle = renderAreaObject.Handle;
      desc.SampleDescription = new SampleDescription(1, 0);
      desc.Usage = Usage.RenderTargetOutput;

      Device dev;
      SwapChain swapChain;
      Device.CreateWithSwapChain(DriverType.Hardware, DeviceCreationFlags.None, desc, out dev, out swapChain);
      Device = dev;
      SwapChain = swapChain;
      Device.Factory.SetWindowAssociation(renderAreaObject.Handle, WindowAssociationFlags.IgnoreAll);

      _dInput = new DirectInput();
      CooperativeLevel cooLevel = CooperativeLevel.Foreground;
      cooLevel |= CooperativeLevel.Nonexclusive;
      _mouse = new Mouse(_dInput);
      _mouse.SetCooperativeLevel(formHandle, cooLevel);

      _shaderByteCode = ShaderBytecode.CompileFromFile(GetFxFile(), "fx_5_0", ShaderFlags.None, EffectFlags.None);
      Effect = new SlimDX.Direct3D11.Effect(Device, _shaderByteCode);
    }

    private ShaderBytecode _shaderByteCode { get; set; }
    public SlimDX.Direct3D11.Effect Effect { get; private set; }

    public Device Device { get; private set; }
    public SwapChain SwapChain { get; private set; }

    public int FrameWidth { get; private set; }
    public int FrameHeight { get; private set; }

    public MouseState GetMouseState()
    {
      MouseState none = new MouseState(0.0f, 0.0f, 0.0f, false, false);

      if (_mouse.Acquire().IsFailure)
      {
        return none;
      }

      if (_mouse.Poll().IsFailure)
      {
        return none;
      }

      SlimDX.DirectInput.MouseState dxState = _mouse.GetCurrentState();

      return new MouseState(dxState.X, dxState.Y, dxState.Z, dxState.IsPressed(0), dxState.IsPressed(1));
    }

    public void Dispose()
    {
      DeInitialize();
    }

    private void DeInitialize()
    {
      SafeDispose(_shaderByteCode);
      SafeDispose(Effect);
      Effect = null;
      SafeDispose(ref _mouse);
      SafeDispose(ref _dInput);
      SafeDispose(Device);
      Device = null;
      SafeDispose(SwapChain);
      SwapChain = null;
    }

    private static void SafeDispose<T>(ref T obj)
      where T : class, IDisposable
    {
      if (obj != null)
      {
        obj.Dispose();
        obj = default(T);
      }
    }

    private static void SafeDispose<T>(T obj)
      where T : class, IDisposable
    {
      if (obj != null)
      {
        obj.Dispose();
      }
    }
  }
}
