using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace MiCoSi
{
  internal class MainFormContext : IDisposable
  {
    private bool _shouldRecreate;
    private bool _recreateSavers;
    private DxRender.DxDeviceContext _devices;
    private DxRender.IDxRenderer _render;
    private States _states;
    private Form _owner;
    private Control _renderArea;

    private SimResultsLoadingAction _srlAction;
    private GuiEventProcessingAction _gepAction;
    private RenderAction _rAction;
    private CaptureAction _cAction;
    private SelectAction _sAction;
    private SelectionPopupFormAction _spfAction;

    public MainFormContext(Form owner,
                           Control renderArea,
                           TrackBar trackBar,
                           Label timeLabel,
                           Button startStopButton,
                           NumericUpDown speedBox,
                           Button screenshotButton,
                           Button videoCaptureButton,
                           Button showPropsButton)
    {
      _shouldRecreate = false;
      _recreateSavers = false;

      _owner = owner;
      _renderArea = renderArea;

      _devices = new DxRender.DxDeviceContext(_owner.Handle, _renderArea, false);
      _render = new DxRender.GeometricDxRenderer(_devices);
      _states = new States();

      _srlAction = new SimResultsLoadingAction(_states);
      _gepAction = new GuiEventProcessingAction(_devices,
                                                _states,
                                                _owner,
                                                _renderArea,
                                                trackBar,
                                                timeLabel,
                                                startStopButton,
                                                speedBox,
                                                screenshotButton,
                                                videoCaptureButton);
      _rAction = new RenderAction(_devices, _render, _states);
      _cAction = new CaptureAction(_devices, _render, _states, SettingsManager.GetRef().CaptureSettings);
      _sAction = new SelectAction(_devices, _render, _states);
      _spfAction = new SelectionPopupFormAction(_states, _owner, showPropsButton);

      if (!_states.IsInitialized)
        throw new ApplicationException("Internal error - failed to register all state providers");
    }

    private void SafeRecreate()
    {
      IOnTickActionContext srlActionContext = _srlAction.DisposeAndExportContext();
      IOnTickActionContext gepActionContext = _gepAction.DisposeAndExportContext();
      IOnTickActionContext rActionContext = _rAction.DisposeAndExportContext();
      IOnTickActionContext cActionContext = _cAction.DisposeAndExportContext();
      IOnTickActionContext sActionContext = _sAction.DisposeAndExportContext();
      IOnTickActionContext spfActionContext = _spfAction.DisposeAndExportContext();

      _states.RemoveStateProviders();

      _render.Dispose();
      _devices.Dispose();

      _devices = new MiCoSi.DxRender.DxDeviceContext(_owner.Handle, _renderArea, false);
      _render = new DxRender.GeometricDxRenderer(_devices);
      
      _srlAction = new SimResultsLoadingAction(_states, srlActionContext);
      _gepAction = new GuiEventProcessingAction(_devices, _states, gepActionContext);
      _rAction = new RenderAction(_devices, _render, _states, rActionContext);
      if (_recreateSavers)
      {
        cActionContext.Dispose();
        _cAction = new CaptureAction(_devices, _render, _states, SettingsManager.GetRef().CaptureSettings);
      }
      else
      {
        _cAction = new CaptureAction(_devices, _render, _states, cActionContext);
      }

      _sAction = new SelectAction(_devices, _render, _states, sActionContext);
      _spfAction = new SelectionPopupFormAction(_states, spfActionContext);

      if (!_states.IsInitialized)
      {
        throw new ApplicationException("Internal error - failed to register all state providers");
      }
    }

    public void Open(String cellFile)
    {
      Monitor.Enter(this);
      try
      { _srlAction.Open(cellFile); }
      finally
      { Monitor.Exit(this); }
    }

    public void Close()
    {
      Monitor.Enter(this);
      try
      { _srlAction.Close(); }
      finally
      { Monitor.Exit(this); }
    }

    public void Recreate(bool recreateSavers)
    {
      _shouldRecreate = true;
      _recreateSavers = recreateSavers;
    }

    public void OnTick()
    {
      Monitor.Enter(this);
      try
      {
        if (_shouldRecreate)
        {
          SafeRecreate();
          _shouldRecreate = false;
        }

        _srlAction.Perform();
        _gepAction.Perform();
        _rAction.Perform();
        _cAction.Perform();
        _sAction.Perform();
        _spfAction.Perform();
      }
      finally
      { Monitor.Exit(this); }
    }

    public bool Paused
    {
      get
      {
        bool res;
        Monitor.Enter(this);
        try
        { res = _gepAction.Paused; }
        finally
        { Monitor.Exit(this); }
        return res;
      }
      set
      {
        Monitor.Enter(this);
        try
        { _gepAction.Paused = value; }
        finally
        { Monitor.Exit(this); }
      }
    }

    public bool SimResultsLoaded
    {
      get
      {
        bool res;
        Monitor.Enter(this);
        try
        { res = _srlAction.SimResultsLoaded; }
        finally
        { Monitor.Exit(this); }
        return res;
      }
    }

    private void SafeRelease()
    {
      if (_srlAction != null)
        _srlAction.Dispose();
      _srlAction = null;
      if (_gepAction != null)
        _gepAction.Dispose();
      _gepAction = null;
      if (_rAction != null)
        _rAction.Dispose();
      _rAction = null;
      if (_cAction != null)
        _cAction.Dispose();
      _cAction = null;
      if (_sAction != null)
        _sAction.Dispose();
      _sAction = null;
      if (_spfAction != null)
        _spfAction.Dispose();
      _spfAction = null;

      if (_render != null)
        _render.Dispose();
      _render = null;
      if (_devices != null)
        _devices.Dispose();
      _devices = null;
    }

    public void Dispose()
    {
      Monitor.Enter(this);
      Monitor.Exit(this);
      SafeRelease();
    }

    ~MainFormContext()
    { SafeRelease(); }
  }
}
