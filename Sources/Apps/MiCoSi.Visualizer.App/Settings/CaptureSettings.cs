using System;
using System.Collections.Generic;
using System.Text;

namespace MiCoSi
{
  [Serializable]
  internal class CaptureSettings : SettingsObject
  {
    public CaptureOptions.ImageSize ImageSize { get; set; }
    public CaptureOptions.ImageContainer ImageContainer { get; set; }

    public CaptureOptions.VideoSize VideoSize { get; set; }
    public CaptureOptions.VideoFPS VideoFPS { get; set; }
    public CaptureOptions.VideoCompression VideoCompression { get; set; }

    public String NamePrefix { get; set; }
    public String OutDirectory { get; set; }

    public CaptureSettings()
    { Reset(); }

    public override void Reset()
    {
      ImageContainer = CaptureOptions.ImageContainer.Png;
      ImageSize = CaptureOptions.ImageSize.size1024x768;

      VideoSize = CaptureOptions.VideoSize.size1280x720;
      VideoCompression = CaptureOptions.VideoCompression.Optimal;
      VideoFPS = CaptureOptions.VideoFPS.fps25;

      NamePrefix = "Mitosis";
      OutDirectory = Directories.GetTempDirectory();
    }

    public override object Clone()
    {
      CaptureSettings res = new CaptureSettings();

      res.ImageSize = ImageSize;
      res.ImageContainer = ImageContainer;
      res.VideoSize = VideoSize;
      res.VideoFPS = VideoFPS;
      res.VideoCompression = VideoCompression;
      res.NamePrefix = NamePrefix;
      res.OutDirectory = OutDirectory;

      return res;
    }
  }
}
