using System;

namespace MiCoSi
{
  [Serializable]
  internal class GuiSettings : SettingsObject
  {
    public int WindowTop { get; set; }
    public int WindowLeft { get; set; }
    public int WindowWidth { get; set; }
    public int WindowHeight { get; set; }
    public decimal Speed { get; set; }
    public String DefaultCellFile { get; set; }

    public bool ShowProperties { get; set; }
    public bool PinProperties { get; set; }
    public int PropertiesTop { get; set; }
    public int PropertiesLeft { get; set; }
    public int PropertiesWidth { get; set; }
    public int PropertiesHeight { get; set; }

    internal GuiSettings()
    { Reset(); }

    public sealed override void Reset()
    {
      WindowTop = 100;
      WindowLeft = 100;
      WindowWidth = 500;
      WindowHeight = 400;
      Speed = 1.0m;
      DefaultCellFile = null;

      ShowProperties = false;
      PinProperties = false;
      PropertiesTop = 100;
      PropertiesLeft = 100;
      PropertiesWidth = 200;
      PropertiesHeight = 300;
    }

    public override object Clone()
    {
      GuiSettings res = new GuiSettings();

      res.WindowTop = WindowTop;
      res.WindowLeft = WindowLeft;
      res.WindowWidth = WindowWidth;
      res.WindowHeight = WindowHeight;
      res.Speed = Speed;
      res.DefaultCellFile = DefaultCellFile;

      res.ShowProperties = ShowProperties;
      res.PinProperties = PinProperties;
      res.PropertiesTop = PropertiesTop;
      res.PropertiesLeft = PropertiesLeft;
      res.PropertiesWidth = PropertiesWidth;
      res.PropertiesHeight = PropertiesHeight;

      return res;
    }
  }
}
