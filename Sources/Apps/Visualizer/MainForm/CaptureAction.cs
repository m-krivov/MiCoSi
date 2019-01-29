using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Imaging;

namespace Mitosis
{
  internal class CaptureAction : IOnTickAction
  {
    private class Context : IOnTickActionContext
    {
      public List<IStateProvider> StateProviders { get; private set; }

      public int ImageWidth { get; private set; }
      public int ImageHeight { get; private set; }
      public ImageSaver ImageSaver { get; private set; }

      public int VideoWidth { get; private set; }
      public int VideoHeight { get; private set; }
      public VideoSaver VideoSaver { get; private set; }

      public Context(DxRender.DxDeviceContext devices, CaptureSettings settings)
      {
        IFileNamingTemplate fnt = new DateTimeFileNamingTemplate(settings.OutDirectory, settings.NamePrefix);

        KeyValuePair<int, int> iSize = CaptureOptions.ToValue(settings.ImageSize);
        ImageWidth = iSize.Key;
        ImageHeight = iSize.Value;
        ImageSaver = new ImageSaver(fnt, settings.ImageContainer);

        KeyValuePair<int, int> vSize = CaptureOptions.ToValue(settings.VideoSize);
        VideoWidth = vSize.Key;
        VideoHeight = vSize.Value;
        VideoSaver = new VideoSaver(fnt, settings.VideoSize, settings.VideoFPS, settings.VideoCompression);

        StateProviders = new List<IStateProvider>();
        StateProviders.Add(new DelegateStateProvider<bool>(StateType.IsVideoRecording, IsVideoCapturing));
      }

      private bool IsVideoCapturing()
      { return VideoSaver.IsCapturing; }

      private void SafeRelease()
      {
        if (ImageSaver != null)
          ImageSaver.Dispose();
        ImageSaver = null;

        if (VideoSaver != null)
          VideoSaver.Dispose();
        VideoSaver = null;
      }

      public void Dispose()
      { SafeRelease(); }

      ~Context()
      { SafeRelease(); }
    }

    private States _states;
    private DxRender.DxDeviceContext _devices;
    private DxRender.IDxRenderer _render;

    private DxRender.DxRenderContext _imageRenderContext;
    private DxRender.TextureRenderTargetAspect _imageRttAspect;
    private DxRender.DxRenderContext _videoRenderContext;
    private DxRender.TextureRenderTargetAspect _videoRttAspect;

    private Context _context;

    public CaptureAction(DxRender.DxDeviceContext devices,
                         DxRender.IDxRenderer render,
                         States states,
                         IOnTickActionContext context)
    {
      _states = states;
      _devices = devices;
      _render = render;

      _context = context as Context;

      _imageRttAspect
        = new DxRender.TextureRenderTargetAspect(devices, _context.ImageWidth, _context.ImageHeight);
      _imageRenderContext = new Mitosis.DxRender.DxRenderContext(new DxRender.DefaultStyleAspect(), _imageRttAspect);

      _videoRttAspect
        = new Mitosis.DxRender.TextureRenderTargetAspect(devices, _context.VideoWidth, _context.VideoHeight);
      _videoRenderContext = new Mitosis.DxRender.DxRenderContext(new DxRender.DefaultStyleAspect(), _videoRttAspect);

      foreach (IStateProvider stateProvider in _context.StateProviders)
        _states.RegisterStateProvider(stateProvider);
    }

    public CaptureAction(DxRender.DxDeviceContext devices,
                         DxRender.IDxRenderer render,
                         States states,
                         CaptureSettings settings)
      : this(devices, render, states, new Context(devices, settings))
    { }

    public void Perform()
    {
      if (_states.VideoCaptureRequest != null)
      {
        if (_context.VideoSaver.IsCapturing)
          _context.VideoSaver.Stop();
        else
          _context.VideoSaver.Start();
      }

      if (_states.ScreenshotRequest != null)
      {
        _render.Camera = _states.Camera;
        _render.Draw(_imageRenderContext, _states.Cell, _states.SimParams);

        IntPtr data;
        int width, height, pitch;
        _imageRttAspect.Lock(out data, out width, out height, out pitch);
        try
        { _context.ImageSaver.Save(data, width, height, pitch, PixelFormat.Format32bppArgb); }
        finally
        { _imageRttAspect.UnLock(); }
      }

      if (_context.VideoSaver.IsCapturing)
      {
        _render.Camera = _states.Camera;
        _render.Draw(_videoRenderContext, _states.Cell, _states.SimParams);

        IntPtr data;
        int width, height, pitch;
        _videoRttAspect.Lock(out data, out width, out height, out pitch);
        try
        { _context.VideoSaver.SetFrame(data, width, height, pitch, PixelFormat.Format32bppArgb); }
        finally
        { _videoRttAspect.UnLock(); }
      }
    }

    private void SafeRelease()
    {
      if (_context != null)
        _context.Dispose();
      _context = null;

      if (_imageRenderContext != null)
        _imageRenderContext.Dispose();
      _imageRenderContext = null;

      if (_videoRenderContext != null)
        _videoRenderContext.Dispose();
      _videoRenderContext = null;

    }

    public IOnTickActionContext DisposeAndExportContext()
    {
      IOnTickActionContext res = _context;
      _context = null;
      SafeRelease();
      return res;
    }

    public void Dispose()
    { SafeRelease(); }

    ~CaptureAction()
    { SafeRelease(); }
  }
}
