using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  internal class SettingsManager : SettingsManagerBase
  {
    private static SettingsManager _ref = new SettingsManager();

    public static SettingsManager GetRef()
    { return _ref; }

    public SettingsManager()
      : base(Directories.GetSettingsFile())
    {
      RegisterObject("GuiSettings", new GuiSettings());
      RegisterObject("RenderSettings", new RenderSettings());
      RegisterObject("CaptureSettings", new CaptureSettings());
    }

    public GuiSettings GuiSettings
    {
      get
      { return GetObject("GuiSettings") as GuiSettings; }
    }

    public RenderSettings RenderSettings
    {
      get
      { return GetObject("RenderSettings") as RenderSettings; }
    }

    public CaptureSettings CaptureSettings
    {
      get
      { return GetObject("CaptureSettings") as CaptureSettings; }
    }
  }
}
