
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
using DirectShowLib;
using GenericSampleSourceFilterClasses;
using System.Threading;

namespace MiCoSi
{
  public class FrameBuffer : ImageHandler
  {
    private int _height;
    private int _width;
    private long _fps;

    private IntPtr _inBuffer;         // new images will be stored into this buffer
    private IntPtr _interBuffer;      // magics buffer for synchronization avoiding
    private IntPtr _outBuffer;        // current images wil be extracted from this buffer
    bool _shouldUpdateOutBuffer;      // request to replace out buffer by intermidiate
    private Timer _timer;
    private Semaphore _semaphore = new Semaphore(0, int.MaxValue);

    [DllImport("kernel32.dll", EntryPoint = "CopyMemory", SetLastError = false)]
    private static extern void CopyMemory(IntPtr dest, IntPtr src, uint count);

    // Gets buffer with actual out image
    private IntPtr GetOutBuffer()
    {
      System.Threading.Monitor.Enter(this);
      if (_shouldUpdateOutBuffer)
      {
        IntPtr tmp = _outBuffer;
        _outBuffer = _interBuffer;
        _interBuffer = tmp;
        _shouldUpdateOutBuffer = false;
      }
      System.Threading.Monitor.Exit(this);

      return _outBuffer;
    }

    // Returns buffer for storing image
    private IntPtr LockInBuffer()
    {
      return _inBuffer;
    }

    // Notifies that input image was stored
    private void UnLockBuffer()
    {
      System.Threading.Monitor.Enter(this);
      _shouldUpdateOutBuffer = true;
      IntPtr tmp = _interBuffer;
      _interBuffer = _inBuffer;
      _inBuffer = tmp;
      System.Threading.Monitor.Exit(this);
    }

    private void OnTimer(object state)
    {
      var count = _semaphore.Release();
    }

    public FrameBuffer(long FPS, int width, int height)
    {
      _fps = UNIT / FPS;
      _width = width;
      _height = height;

      _inBuffer = Marshal.AllocHGlobal(4 * width * height);
      _interBuffer = Marshal.AllocHGlobal(4 * width * height);
      _outBuffer = Marshal.AllocHGlobal(4 * width * height);
      _shouldUpdateOutBuffer = false;

      _timer = new Timer(OnTimer);
      _timer.Change(TimeSpan.Zero, TimeSpan.FromSeconds(1.0 / FPS));
    }

    override public void SetMediaType(IGenericSampleConfig psc)
    {
      BitmapInfoHeader bmi = new BitmapInfoHeader();

      // Build a BitmapInfo struct using the parms from the file
      bmi.Size = Marshal.SizeOf(typeof(BitmapInfoHeader));
      bmi.Width = _width;
      bmi.Height = _height * -1;
      bmi.Planes = 1;
      bmi.BitCount = 32;
      bmi.Compression = 0;
      bmi.ImageSize = (bmi.BitCount / 8) * bmi.Width * bmi.Height;
      bmi.XPelsPerMeter = 0;
      bmi.YPelsPerMeter = 0;
      bmi.ClrUsed = 0;
      bmi.ClrImportant = 0;

      int hr = psc.SetMediaTypeFromBitmap(bmi, _fps);
      DsError.ThrowExceptionForHR(hr);
    }

    public void SetImage(IntPtr img, int width, int height, int pitchInBytes)
    {
      if (_width != width || _height != height)
        throw new ApplicationException("Wrong frame size");

      IntPtr buf = LockInBuffer();

      // Copying frame
      for (int y = 0; y < height; y++)
      {
        CopyMemory((IntPtr)((Int64)buf + y * width * 4),
                   (IntPtr)((Int64)img + y * pitchInBytes),
                   (UInt32)(4 * width));
      }

      UnLockBuffer();
    }

    override public int GetImage(int iFrameNumber, IntPtr ip, int iSize, out int iRead)
    {
      _semaphore.WaitOne();

      int hr = 0;

      IntPtr buf = GetOutBuffer();
      CopyMemory(ip, buf, (UInt32)(_width * _height * 4));
      iRead = iSize;

      return hr;
    }

    override public int SetTimeStamps(IMediaSample pSample)
    {
      DsLong rtStart = new DsLong(m_iFrameNumber * _fps);
      DsLong rtStop = new DsLong(rtStart + _fps);

      int hr = pSample.SetTime(rtStart, rtStop);

      return hr;
    }

    public override void Dispose()
    {
      Marshal.FreeHGlobal(_inBuffer);
      _inBuffer = (IntPtr)null;
      Marshal.FreeHGlobal(_interBuffer);
      _interBuffer = (IntPtr)null;
      Marshal.FreeHGlobal(_outBuffer);
      _outBuffer = (IntPtr)null;

      base.Dispose();

      if (_timer != null)
      {
        _timer.Dispose();
      }

      if (_semaphore != null)
      {
        _semaphore.Close();
      }
    }
  }
}