using System;
using System.Collections.Generic;
using System.Text;

namespace MiCoSi
{
  internal class SelectAction : IOnTickAction
  {
    private class Context : IOnTickActionContext
    {
      public List<IStateProvider> StateProviders { get; private set; }

      public Object SelectedObject { get; set; }

      public Context()
      {
        StateProviders = new List<IStateProvider>();

        StateProviders.Add(new DelegateStateProvider<Object>(StateType.SelectedObject, GetSelectedObject));
      }

      private Object GetSelectedObject()
      { return SelectedObject; }

      public void Dispose()
      { /*nothing*/ }
    }

    private Context _context;
    private readonly DxRender.DxDeviceContext _devices;
    private readonly DxRender.IDxRenderer _render;
    private DxRender.DxRenderContext _renderContext;
    private readonly DxRender.TextureRenderTargetAspect _rtAspect;
    private readonly DxRender.ObjectSelectingStyleAspect _stAspect;
    private readonly States _states;

    public SelectAction(DxRender.DxDeviceContext devices,
                        DxRender.IDxRenderer render,
                        States states)
      : this(devices, render, states, new Context())
    { /*nothing*/ }

    public SelectAction(DxRender.DxDeviceContext devices,
                        DxRender.IDxRenderer render,
                        States states,
                        IOnTickActionContext context)
    {
      _context = context as Context;

      _devices = devices;
      _render = render;
      _states = states;

      _rtAspect
        = new MiCoSi.DxRender.TextureRenderTargetAspect(_devices, _devices.FrameWidth, _devices.FrameHeight);
      _stAspect = new MiCoSi.DxRender.ObjectSelectingStyleAspect();

      _renderContext = new MiCoSi.DxRender.DxRenderContext(_stAspect, _rtAspect);

      foreach (var stateProvider in _context.StateProviders)
      { _states.RegisterStateProvider(stateProvider); }
    }

    public void Perform()
    {
      if (_states.SelectionRequest != null)
      {
        _render.Camera = _states.Camera.Clone() as Camera;
        _render.Draw(_renderContext, _states.Cell, _states.SimParams);

        IntPtr data;
        int width, height, pitch;
        _rtAspect.Lock(out data, out width, out height, out pitch);
        try
        {
          KeyValuePair<int, int> coords = _states.MouseLocalCoordinates;
          int mouseX = Math.Max(0, Math.Min(coords.Key, width - 1));
          int mouseY = Math.Max(0, Math.Min(coords.Value, height - 1));

          unsafe
          {
            int color = *((int*)data + mouseX + mouseY * pitch / 4);
            _context.SelectedObject = _stAspect.DecodeColor(_states.Cell, new SlimDX.Color4(color));
          }
        }
        finally
        { _rtAspect.UnLock(); }
      }
    }

    public IOnTickActionContext DisposeAndExportContext()
    {
      Context res = _context;
      _context = null;
      Dispose();
      return res;
    }

    private void SafeRelease()
    {
      if (_renderContext != null)
        _renderContext.Dispose();
      _renderContext = null;

      if (_context != null)
        _context.Dispose();
      _context = null;
    }

    public void Dispose()
    { SafeRelease(); }

    ~SelectAction()
    { SafeRelease(); }
  }
}
