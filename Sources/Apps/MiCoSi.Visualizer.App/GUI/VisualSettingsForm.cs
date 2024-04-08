using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MiCoSi
{
  public partial class VisualSettingsForm : Form
  {
    private bool _shouldApply;

    private VisualSettingsForm(List<String> styles, int selectedStyle, bool antiAliasing, bool textures)
    {
      InitializeComponent();

      _shouldApply = false;
      cbAntiAliasing.Checked = antiAliasing;
      cbTextures.Checked = textures;
      cbStyle.Items.Clear();
      foreach (var style in styles)
        cbStyle.Items.Add(style);
      cbStyle.SelectedIndex = Math.Min(styles.Count - 1, Math.Max(0, selectedStyle));
    }

    static internal bool CreateAndShow(List<String> styles, RenderSettings settings)
    {
      int styleIndex = -1;
      for (int i = 0; i < styles.Count; i++)
        if (styles[i].ToLower() == settings.StyleName.ToLower())
        {
          styleIndex = i;
          break;
        }
      if (styleIndex < 0)
      {
        UserErrors.SettingsFileCorrupted("using default visual settings");
        styleIndex = 0;
      }

      VisualSettingsForm form = new VisualSettingsForm(styles,
                                                       styleIndex,
                                                       settings.AntiAliasingEnabled,
                                                       settings.TexturesEnabled);
      form.ShowDialog();

      if (form._shouldApply)
      {
        styleIndex = Math.Min(styles.Count - 1, Math.Max(0, form.cbStyle.SelectedIndex));
        settings.StyleName = styles[styleIndex];
        settings.AntiAliasingEnabled = form.cbAntiAliasing.Checked;
        settings.TexturesEnabled = form.cbTextures.Checked;
        return true;
      }
      else
        return false;
    }

    private void btApply_Click(object sender, EventArgs e)
    {
      _shouldApply = true;
      Close();
    }
  }
}
