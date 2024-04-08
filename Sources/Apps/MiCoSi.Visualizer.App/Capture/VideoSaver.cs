using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Imaging;

namespace MiCoSi
{
  internal class VideoSaver : IDisposable
  {
    private DxPlay _dxPlay;
    private FrameBuffer _buffer;
    private IFileNamingTemplate _fileNaming;
    private int _frameWidth;
    private int _frameHeight;
    private int _FPS;
    private CaptureOptions.VideoCompression _compression;

    public VideoSaver(IFileNamingTemplate fileNaming,
                      CaptureOptions.VideoSize frameSize,
                      CaptureOptions.VideoFPS FPS,
                      CaptureOptions.VideoCompression compression)
    {
      KeyValuePair<int, int> size = CaptureOptions.ToValue(frameSize);
      
      _fileNaming = fileNaming;
      _frameWidth = size.Key;
      _frameHeight = size.Value;
      _FPS = CaptureOptions.ToValue(FPS);
      _compression = compression;

      _buffer = null;
      _dxPlay = null;
    }

    public bool IsCapturing
    {
      get
      { return _dxPlay != null; }
    }

    public void Start()
    {
      if (_dxPlay == null)
      {
        _buffer = new FrameBuffer(_FPS, _frameWidth, _frameHeight);
        _dxPlay = new DxPlay(_buffer,
                             _fileNaming.Generate(".wmv"),
                             _compression,
                             _frameWidth,
                             _frameHeight,
                             _FPS);
        _dxPlay.Start();
      }
    }

    public void SetFrame(IntPtr data, int width, int height, int pitchInBytes, PixelFormat format)
    {
      if (format != PixelFormat.Format32bppArgb)
        throw new ApplicationException("Unsupported pixel format");
      
      _buffer.SetImage(data, width, height, pitchInBytes);
    }

    public void Stop()
    {
      if (_dxPlay != null)
      {
        _dxPlay.Stop();
        _dxPlay.Dispose();
        _buffer.Dispose();
      }
      _dxPlay = null;
      _buffer = null;
    }

    public void Dispose()
    {
      Stop();

      if (_buffer != null)
        _buffer.Dispose();
      _buffer = null;
    }
  }
}
