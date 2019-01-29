using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  internal class RenderAction : IOnTickAction
  {
    private readonly DxRender.DxDeviceContext _devices;
    private readonly DxRender.IDxRenderer _render;
    private DxRender.DxRenderContext _renderContext;
    private readonly DxRender.DefaultStyleAspect _stAspect;
    private readonly DxRender.ScreenRenderTargetAspect _rtAspect;
    private readonly States _states;

    public RenderAction(DxRender.DxDeviceContext devices,
                        DxRender.IDxRenderer render,
                        States states)
    {
      _devices = devices;
      _render = render;
      _states = states;

      _stAspect = new DxRender.DefaultStyleAspect();
      _rtAspect = new DxRender.ScreenRenderTargetAspect(_devices);
      _renderContext = new Mitosis.DxRender.DxRenderContext(_stAspect, _rtAspect);
    }

    public RenderAction(DxRender.DxDeviceContext devices,
                        DxRender.IDxRenderer render,
                        States states,
                        IOnTickActionContext context)
      : this(devices, render, states)
    { }

    public void Perform()
    {
      _rtAspect.ShowRedDot = _states.IsVideoRecording;
      _render.Camera = _states.Camera.Clone() as Camera;
      _stAspect.SelectedObject = _states.SelectedObject;
      _render.Draw(_renderContext, _states.Cell, _states.SimParams);
    }

    private void SafeRelease()
    {
      if (_renderContext != null)
      {
        _renderContext.Dispose();
      }
      _renderContext = null;
    }

    public IOnTickActionContext DisposeAndExportContext()
    {
      Dispose();
      return null;
    }

    public void Dispose()
    { SafeRelease(); }

    ~RenderAction()
    { SafeRelease(); }
  }
}
