using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  [Serializable]
  internal class RenderSettings : SettingsObject
  {
    public String StyleName { get; set; }
    public bool AntiAliasingEnabled { get; set; }
    public bool TexturesEnabled { get; set; }

    internal RenderSettings()
    { Reset(); }

    public override void Reset()
    {
      StyleName = "Default";
      AntiAliasingEnabled = false;
      TexturesEnabled = false;
    }

    public override object Clone()
    {
      RenderSettings res = new RenderSettings();

      res.StyleName = StyleName;
      res.AntiAliasingEnabled = AntiAliasingEnabled;
      res.TexturesEnabled = TexturesEnabled;

      return res;
    }
  }
}
