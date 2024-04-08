using System;
using System.Collections.Generic;
using System.Text;

namespace MiCoSi
{
  public enum StateType
  {
    //GUI states
    ScreenshotRequest,
    VideoCaptureRequest,
    IsVideoRecording,
    MouseLocalCoordinates,
    SelectonRequest,
    SelectedObject,
    IsPlaybackPaused,
    PlaybackSpeed,
    ScrollRequest,

    //Simulaton playback states
    Cell,
    SimParams,
    CurrentSimTime,
    EndSimTime,

    //Render states
    Camera
  }

  internal interface IStateProvider
  {
    IEnumerable<StateType> GetStateIDs();
  }

  internal interface IGenericStateProvider<T> : IStateProvider
  {
    T GetState(StateType stateID);
  }

  delegate T StateProviderDelegate<T>();

  internal class DelegateStateProvider<T> : IGenericStateProvider<T>
  {
    private List<StateType> _ids;
    private StateProviderDelegate<T> _stDelegate;

    public DelegateStateProvider(StateType stateID, StateProviderDelegate<T> stDelegate)
    {
      _ids = new List<StateType>();
      _ids.Add(stateID);
      _stDelegate = stDelegate;
    }

    public T GetState(StateType stateID)
    {
      return _stDelegate();
    }

    public IEnumerable<StateType> GetStateIDs()
    {
      return _ids;
    }
  }

  internal class States
  {
    public static List<StateType> StateIDs { get; private set; }
    
    static States()
    {
      StateIDs = new List<StateType>();

      StateIDs.Add(StateType.ScreenshotRequest);
      StateIDs.Add(StateType.VideoCaptureRequest);
      StateIDs.Add(StateType.IsVideoRecording);
      StateIDs.Add(StateType.MouseLocalCoordinates);
      StateIDs.Add(StateType.SelectonRequest);
      StateIDs.Add(StateType.SelectedObject);
      StateIDs.Add(StateType.IsPlaybackPaused);
      StateIDs.Add(StateType.PlaybackSpeed);
      StateIDs.Add(StateType.ScrollRequest);

      StateIDs.Add(StateType.Cell);
      StateIDs.Add(StateType.SimParams);
      StateIDs.Add(StateType.CurrentSimTime);
      StateIDs.Add(StateType.EndSimTime);

      StateIDs.Add(StateType.Camera);
    }

    private Dictionary<StateType, IStateProvider> _providers;

    public States()
    {
      _providers = new Dictionary<StateType, IStateProvider>();
    }

    private IStateProvider FindStateProvider(StateType stateID)
    { return _providers[stateID]; }

    public Object ScreenshotRequest
    {
      get
      {
        return (FindStateProvider(StateType.ScreenshotRequest) as IGenericStateProvider<Object>)
          .GetState(StateType.ScreenshotRequest);
      }
    }

    public Object VideoCaptureRequest
    {
      get
      {
        return (FindStateProvider(StateType.VideoCaptureRequest) as IGenericStateProvider<Object>)
          .GetState(StateType.VideoCaptureRequest);
      }
    }

    public bool IsVideoRecording
    {
      get
      {
        return (FindStateProvider(StateType.IsVideoRecording) as IGenericStateProvider<bool>)
          .GetState(StateType.IsVideoRecording);
      }
    }

    public KeyValuePair<int, int> MouseLocalCoordinates
    {
      get
      {
        return (FindStateProvider(StateType.MouseLocalCoordinates) as IGenericStateProvider<KeyValuePair<int, int>>)
          .GetState(StateType.MouseLocalCoordinates);
      }
    }

    public Object SelectionRequest
    {
      get
      {
        return (FindStateProvider(StateType.SelectonRequest) as IGenericStateProvider<Object>)
          .GetState(StateType.SelectonRequest);
      }
    }

    public Object SelectedObject
    {
      get
      {
        return (FindStateProvider(StateType.SelectedObject) as IGenericStateProvider<Object>)
          .GetState(StateType.SelectedObject);
      }
    }

    public bool IsPlaybackPaused
    {
      get
      {
        return (FindStateProvider(StateType.IsPlaybackPaused) as IGenericStateProvider<bool>)
          .GetState(StateType.IsPlaybackPaused);
      }
    }

    public double PlaybackSpeed
    {
      get
      {
        return (FindStateProvider(StateType.PlaybackSpeed) as IGenericStateProvider<double>)
          .GetState(StateType.PlaybackSpeed);
      }
    }

    public Object ScrollRequest
    {
      get
      {
        return (FindStateProvider(StateType.ScrollRequest) as IGenericStateProvider<Object>)
          .GetState(StateType.ScrollRequest);
      }
    }

    public Cell Cell
    {
      get
      {
        return (FindStateProvider(StateType.Cell) as IGenericStateProvider<Cell>)
          .GetState(StateType.Cell);
      }
    }

    public SimParams SimParams
    {
      get
      {
        return (FindStateProvider(StateType.SimParams) as IGenericStateProvider<SimParams>)
             .GetState(StateType.SimParams);
      }
    }

    public double CurrentSimTime
    {
      get
      {
        return (FindStateProvider(StateType.CurrentSimTime) as IGenericStateProvider<double>)
             .GetState(StateType.CurrentSimTime);
      }
    }

    public double EndSimTime
    {
      get
      {
        return (FindStateProvider(StateType.EndSimTime) as IGenericStateProvider<double>)
             .GetState(StateType.EndSimTime);
      }
    }

    public Camera Camera
    {
      get
      {
        return (FindStateProvider(StateType.Camera) as IGenericStateProvider<Camera>)
             .GetState(StateType.Camera);
      }
    }

    public void RegisterStateProvider(IStateProvider provider)
    {
      foreach (var stateID in provider.GetStateIDs())
        _providers[stateID] = provider;
    }

    public void RemoveStateProviders()
    { _providers.Clear(); }

    public bool IsInitialized
    {
      get
      {
        IStateProvider val;
        foreach (StateType state in StateIDs)
          if (!_providers.TryGetValue(state, out val))
            return false;
        return true;
      }
    }
  }
}
