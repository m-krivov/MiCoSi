using System;

namespace Mitosis.DxRender
{
  internal class DefaultStyleAspect : IStyleAspect
  {
    private readonly StyleCollection _unSelected;
    private readonly StyleCollection _selected;

    public DefaultStyleAspect()
    {
      // No selection by default.
      SelectedObject = null;

      // Loading required style for unselected objects.
      try
      { _unSelected = StyleManager.CreateByName(SettingsManager.GetRef().RenderSettings.StyleName); }
      catch (Exception)
      {
        UserErrors.SettingsFileCorrupted("using default style");
        SettingsManager.GetRef().RenderSettings.StyleName = StyleManager.DefaultName;
        _unSelected = StyleManager.CreateByName(StyleManager.DefaultName);
      }

      // Creating style for selected objects.
      _selected = _unSelected.Clone() as StyleCollection;
      Material selectedMat = new Material(new SlimDX.Vector4(0.9f, 0.1f, 0.1f, 1.0f),
                                          new SlimDX.Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                                          new SlimDX.Vector4(0.05f, 0.05f, 0.05f, 1.0f));
      foreach (var style in _selected.AllStyles)
      { style.SetProperties(selectedMat); }
    }

    public Object SelectedObject { get; set; }

    public string TechniqueName
    {
      get
      { return "Default"; }
    }

    public T Resolve<T>(IObjectWithID obj)
      where T : StyleObject
    {
      return (obj.Equals(SelectedObject) ? _selected : _unSelected)
        .AllStyles.Find(styleObject => styleObject.GetType() == typeof(T)) as T;
    }
  }
}
