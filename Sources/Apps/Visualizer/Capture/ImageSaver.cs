using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;

namespace Mitosis
{
  internal class ImageSaver : IDisposable
  {
    private IFileNamingTemplate _fileNaming;
    private String _fileExt;
    private ImageFormat _format;

    public ImageSaver(IFileNamingTemplate fileNaming, CaptureOptions.ImageContainer container)
    {
      _fileNaming = fileNaming;

      var formatAndExt = CaptureOptions.ToValue(container);
      _format = formatAndExt.Key;
      _fileExt = formatAndExt.Value;
    }

    public void Save(IntPtr data, int width, int height, int pitchInBytes, PixelFormat format)
    {
      String outFile = _fileNaming.Generate(_fileExt);
      
      using (Bitmap bmp = new Bitmap(width, height, pitchInBytes, format, data))
      {
        bmp.Save(outFile, _format);
      }
    }

    public void Dispose()
    { /*nothing*/ }
  }
}
