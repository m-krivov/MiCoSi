using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  internal class SimResultsLoadingAction : IOnTickAction
  {
    private class Context : IOnTickActionContext
    {
      public List<IStateProvider> StateProviders { get; private set; }
      public TimeStream TimeStream { get; set; }
      public System.Diagnostics.Stopwatch Timer { get; set; }
      public int CurrentLayer { get; set; }
      public double CurrentTime { get; set; }

      public Context()
      {
        StateProviders = new List<IStateProvider>();
        StateProviders.Add(new DelegateStateProvider<double>(StateType.CurrentSimTime, GetCurrentTime));
        StateProviders.Add(new DelegateStateProvider<double>(StateType.EndSimTime, GetEndTime));
        StateProviders.Add(new DelegateStateProvider<Cell>(StateType.Cell, GetCell));
        StateProviders.Add(new DelegateStateProvider<SimParams>(StateType.SimParams, GetSimParams));

        TimeStream = null;
        Timer = null;
        CurrentLayer = 0;
        CurrentTime = 0.0;
      }

      private double GetCurrentTime()
      { return TimeStream == null ? 0.0 : CurrentTime; }

      private double GetEndTime()
      { return TimeStream == null ? 0.0 : TimeStream.GetLayerTime(TimeStream.LayerCount - 1); }

      private Cell GetCell()
      { return TimeStream == null ? null : TimeStream.Current.Cell; }

      private SimParams GetSimParams()
      { return TimeStream == null ? null : TimeStream.Current.SimParams; }

      private void SafeRelease()
      {
        if (TimeStream != null)
          TimeStream.Dispose();
        TimeStream = null;
      }

      public void Dispose()
      { SafeRelease(); }

      ~Context()
      { SafeRelease(); }
    }

    private States _states;
    private Context _context;

    public SimResultsLoadingAction(States states, IOnTickActionContext context)
    {
      _states = states;
      _context = context as Context;
      
      foreach (var provider in _context.StateProviders)
        _states.RegisterStateProvider(provider);
    }

    public SimResultsLoadingAction(States states)
      : this(states, new Context())
    {
      _context.TimeStream = null;
      _context.CurrentLayer = 0;
      _context.CurrentTime = 0;
      _context.Timer = new System.Diagnostics.Stopwatch();
    }

    public void Perform()
    {
      if (_context.TimeStream == null)
        return;

      _context.Timer.Stop();
      if ((!_states.IsPlaybackPaused && _context.CurrentLayer != _context.TimeStream.LayerCount - 1) ||
        _states.ScrollRequest != null)
      {
        int prevLayer = _context.CurrentLayer;
        double endTime = _context.TimeStream.GetLayerTime(_context.TimeStream.LayerCount - 1);

        if (_states.ScrollRequest == null)
        { _context.CurrentTime += _context.Timer.ElapsedMilliseconds * 1e-3 * _states.PlaybackSpeed; }
        else
        {
          _context.CurrentLayer = 0;
          _context.CurrentTime = Math.Min(Math.Max(0.0, (double)_states.ScrollRequest), endTime);
        }

        while (_context.CurrentLayer < _context.TimeStream.LayerCount - 1 &&
             _context.CurrentTime > _context.TimeStream.GetLayerTime(_context.CurrentLayer + 1))
        { _context.CurrentLayer += 1; }
        _context.CurrentTime = Math.Min(_context.CurrentTime, endTime);

        if (prevLayer != _context.CurrentLayer)
          _context.TimeStream.MoveTo(_context.CurrentLayer);
      }
      _context.Timer.Reset();
      _context.Timer.Start();
    }

    public void Open(String cellFile)
    {
      if (_context.TimeStream != null)
        Close();

      _context.TimeStream = TimeStream.Open(cellFile);
      _context.TimeStream.MoveTo(0);
      _context.CurrentLayer = 0;
      _context.CurrentTime = 0.0;
      _context.Timer.Stop();
      _context.Timer.Reset();
    }

    public bool SimResultsLoaded
    {
      get
      { return _context.TimeStream != null; }
    }

    public void Close()
    {
      if (_context.TimeStream != null)
      {
        _context.TimeStream.Dispose();
        _context.TimeStream = null;
        _context.CurrentLayer = 0;
        _context.CurrentTime = 0.0;
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

    ~SimResultsLoadingAction()
    { SafeRelease(); }
  }
}
