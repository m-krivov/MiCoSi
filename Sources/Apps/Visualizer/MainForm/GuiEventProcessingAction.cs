
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Mitosis
{
  internal class GuiEventProcessingAction : IOnTickAction
  {
    private class Context : IOnTickActionContext
    {
      private static String _playString = "Play";
      private static String _stopString = "Pause";

      private static String FormatTimeString(double timeInSeconds)
      {
        int intTime = (int)timeInSeconds;
        int seconds = intTime % 60;
        int minutes = (intTime / 60) % 60;
        int hours = (intTime / 3600) % 60;
        if (hours == 0)
          return String.Format("{0}:{1}", minutes.ToString("00"), seconds.ToString("00"));
        else
          return String.Format("{0}:{1}:{2}", hours, minutes.ToString("00"), seconds.ToString("00"));
      }

      private static String FormatTotalTimeString(double curTime, double totalTime)
      { return FormatTimeString(curTime) + @" / " + FormatTimeString(totalTime); }

      private Form _owner;
      private TrackBar _trackBar;
      private Label _timeLabel;
      private Button _startStopButton;
      private NumericUpDown _speedBox;
      private Button _screenshotButton;
      private Button _videoCaptureButton;

      public Camera Camera { get; set; }
      public List<IStateProvider> StateProviders { get; private set; }
      public double TotalTime { get; private set; }
      public Control RenderArea { get; private set; }
      
      public Object AccumulatedScrollRequests { get; set; }
      public Object ScrollRequest { get; set; }
      public Object AccumulatedScreenshotRequests { get; set; }
      public Object ScreenshotRequest { get; set; }
      public Object AccumulatedVideoCaptureRequests { get; set; }
      public Object VideoCaptureRequest { get; set; }
      public Object SelectionRequest { get; set; }
      
      public bool Paused
      {
        get
        { return _startStopButton.Text == _playString; }
        set
        {
          if (value != Paused)
            _startStopButton.PerformClick();
        }
      }

      public double NormalizedTime
      {
        get
        { return (double)(_trackBar.Value - _trackBar.Minimum) / (_trackBar.Maximum - _trackBar.Minimum); }
        set
        {
          double realTime = Math.Min(TotalTime, value * TotalTime);
          _trackBar.Value = Math.Max(_trackBar.Minimum,
                                     Math.Min(_trackBar.Maximum,
                                              (int)(_trackBar.Minimum +
                                              value * (_trackBar.Maximum - _trackBar.Minimum))));
          _timeLabel.Text = FormatTotalTimeString(realTime, TotalTime);
        }
      }

      private void OnPausePlayClicked(Object sender, EventArgs args)
      { _startStopButton.Text = _startStopButton.Text == _playString ? _stopString : _playString; }

      private void OnScreenshotRequest(Object sender, EventArgs args)
      { AccumulatedScreenshotRequests = (Object)true; }

      private void OnVideoCaptureRequest(Object sender, EventArgs args)
      { AccumulatedVideoCaptureRequests = AccumulatedVideoCaptureRequests == null ? (Object)true : null; }

      private void OnScrolled(Object sender, EventArgs args)
      { AccumulatedScrollRequests = NormalizedTime * TotalTime; }

      public Context(Form owner,
                     Control renderArea,
                     TrackBar trackBar,
                     Label timeLabel,
                     Button startStopButton,
                     NumericUpDown speedBox,
                     Button screenshotButton,
                     Button videoCaptureButton)
      {
        _owner = owner;
        RenderArea = renderArea;
        
        _trackBar = trackBar;
        _timeLabel = timeLabel;
        _startStopButton = startStopButton;
        _speedBox = speedBox;
        _screenshotButton = screenshotButton;
        _videoCaptureButton = videoCaptureButton;

        _speedBox.Value = SettingsManager.GetRef().GuiSettings.Speed;
        _trackBar.Scroll += OnScrolled;
        _startStopButton.Click += OnPausePlayClicked;
        _screenshotButton.Click += OnScreenshotRequest;
        _videoCaptureButton.Click += OnVideoCaptureRequest;

        Camera = new Camera();

        StateProviders = new List<IStateProvider>();
        StateProviders.Add(new DelegateStateProvider<Object>(StateType.ScreenshotRequest, GetScreenshotRequest));
        StateProviders.Add(new DelegateStateProvider<Object>(StateType.VideoCaptureRequest, GetVideoCaptureRequest));
        StateProviders.Add(
          new DelegateStateProvider<KeyValuePair<int, int>>(StateType.MouseLocalCoordinates, GetMouseLocalCoordinates));
        StateProviders.Add(new DelegateStateProvider<bool>(StateType.IsPlaybackPaused, IsPlaybackPaused));
        StateProviders.Add(new DelegateStateProvider<double>(StateType.PlaybackSpeed, GetPlaybackSpeed));
        StateProviders.Add(new DelegateStateProvider<Object>(StateType.ScrollRequest, GetScrollRequest));
        StateProviders.Add(new DelegateStateProvider<Camera>(StateType.Camera, GetCamera));
        StateProviders.Add(new DelegateStateProvider<Object>(StateType.SelectonRequest, GetSelectionRequest));

        TotalTime = 0.0;
      }

      private Object GetScreenshotRequest()
      { return ScreenshotRequest; }

      private Object GetVideoCaptureRequest()
      { return VideoCaptureRequest; }

      private bool IsPlaybackPaused()
      { return Paused; }

      private double GetPlaybackSpeed()
      { return (double)_speedBox.Value; }

      private Object GetScrollRequest()
      { return ScrollRequest; }

      private Object GetSelectionRequest()
      { return SelectionRequest; }

      private Camera GetCamera()
      { return Camera; }


      private KeyValuePair<int, int> GetMouseLocalCoordinates()
      {
        System.Drawing.Point coords = RenderArea.PointToClient(Control.MousePosition);

        return new KeyValuePair<int, int>(Math.Min(Math.Max(0, coords.X), RenderArea.Width - 1),
                                          Math.Min(Math.Max(0, coords.Y), RenderArea.Height - 1));
      }

      public void DisableControls()
      {
        _trackBar.Minimum = 0;
        _trackBar.Maximum = 0;
        _trackBar.Enabled = false;

        _timeLabel.Text = FormatTotalTimeString(0.0, 0.0);

        _startStopButton.Text = _playString;
        _startStopButton.Enabled = false;

        _speedBox.Enabled = false;

        TotalTime = 0.0;
      }

      public void EnableControls(double totalTime)
      {
        if (totalTime == 0.0)
        {
          DisableControls();
          return;
        }

        _trackBar.Minimum = 0;
        _trackBar.Maximum = 1000;
        _trackBar.Enabled = true;

        _timeLabel.Text = FormatTotalTimeString(0.0, totalTime);

        _startStopButton.Text = _stopString;
        _startStopButton.Enabled = true;

        _speedBox.Enabled = true;

        TotalTime = totalTime;
      }

      public void Dispose()
      {
        /*nothing*/
      }
    }

    private DxRender.DxDeviceContext _devices;
    private States _states;
    private Context _context;

    public GuiEventProcessingAction(DxRender.DxDeviceContext devices,
                                    States states,
                                    IOnTickActionContext context)
    {
      _devices = devices;
      _states = states;
      _context = context as Context;

      foreach (IStateProvider provider in _context.StateProviders)
        _states.RegisterStateProvider(provider);
    }

    public GuiEventProcessingAction(DxRender.DxDeviceContext devices,
                    States states,
                    Form owner,
                    Control renderArea,
                    TrackBar trackBar,
                    Label timeLabel,
                    Button startStopButton,
                    NumericUpDown speedBox,
                    Button screenshotButton,
                    Button videoCaptureButton)
      : this(devices, states, new Context(owner, renderArea, trackBar, timeLabel,
                                          startStopButton, speedBox,
                                          screenshotButton, videoCaptureButton))
    {
      _context.Camera = new Camera(new SlimDX.Vector3(0.0f, 0.0f, -2.0f),
                                   new SlimDX.Vector3(0.0f, 0.0f, 0.0f),
                                   new SlimDX.Vector3(0.0f, 1.0f, 0.0f));

      _context.DisableControls();
    }

    public bool Paused
    {
      get
      { return _context.Paused; }
      set
      { _context.Paused = value; }
    }

    private bool IsMouseOverRenderArea()
    { return _context.RenderArea.ClientRectangle.Contains(_context.RenderArea.PointToClient(Control.MousePosition)); }

    public void Perform()
    {
      _context.ScrollRequest = _context.AccumulatedScrollRequests;
      _context.AccumulatedScrollRequests = null;

      _context.ScreenshotRequest = _context.AccumulatedScreenshotRequests;
      _context.AccumulatedScreenshotRequests = null;

      _context.VideoCaptureRequest = _context.AccumulatedVideoCaptureRequests;
      _context.AccumulatedVideoCaptureRequests = null;

      _context.SelectionRequest = null;

      if (_states.Cell == null)
      {
        if (_context.TotalTime != 0.0)
          _context.DisableControls();
        return;
      }

      if (_context.TotalTime != _states.EndSimTime)
        _context.EnableControls(_states.EndSimTime);
      
      _context.NormalizedTime = _states.CurrentSimTime / (_states.EndSimTime == 0.0 ? 1.0 : _states.EndSimTime);

      if (IsMouseOverRenderArea())
      {
        MouseState mouseState = _devices.GetMouseState();

        if (mouseState.LeftButtonPressed)
        {
          _context.SelectionRequest = (Object)true;
        }

        if (mouseState.RightButtonPressed)
        {
          _context.RenderArea.Cursor = Cursors.SizeAll;
          SlimDX.Vector3 up = _context.Camera.Up;
          SlimDX.Vector3 pos = _context.Camera.Position;
          SlimDX.Vector3 tmp = new SlimDX.Vector3();
          float posLength = pos.Length();

          //Scrolling
          if (posLength - 1e-1 > 0.001f * mouseState.DZ)
          {
            float coeff = 1.0f - 0.001f * mouseState.DZ / posLength;
            pos *= coeff;
            posLength = pos.Length();
          }
          //DX
          tmp = SlimDX.Vector3.Cross(pos, up);
          tmp.Normalize();
          pos -= tmp * mouseState.DX * 0.01f;
          pos.Normalize();
          pos *= posLength;
          //DY
          tmp = SlimDX.Vector3.Cross(pos, up);
          pos += up * mouseState.DY * 0.01f;
          pos.Normalize();
          pos *= posLength;
          up = -SlimDX.Vector3.Cross(pos, tmp);
          up.Normalize();

          _context.Camera.Position = pos;
          _context.Camera.Up = up;
        }
        else
          _context.RenderArea.Cursor = Cursors.Arrow;
      }
    }

    public IOnTickActionContext DisposeAndExportContext()
    {
      IOnTickActionContext res = _context;
      _context = null;
      Dispose();
      return res;
    }

    private void SafeRelease()
    {
      if (_context != null)
        _context.Dispose();
      _context = null;
    }

    public void Dispose()
    { SafeRelease(); }

    ~GuiEventProcessingAction()
    { SafeRelease(); }
  }
}
