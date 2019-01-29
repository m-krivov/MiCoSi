using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;

namespace Mitosis
{
  public partial class SelectionPopupForm : Form
  {
    private class Record
    {
      public Label Label { get; private set; }
      public TextBox TextBox { get; private set; }

      public Record(Label label, TextBox textBox)
      {
        Label = label;
        TextBox = textBox;
      }
    }

    private const int Delta = 10;
    private const String NoSelection = "No selection";
    private readonly EventHandler _onClosing;
    private List<Record> _records;
    private int _hashCode;

    public SelectionPopupForm(EventHandler onClosing)
    {
      InitializeComponent();
      SetNoSelection();

      _onClosing = onClosing;

      Width = SettingsManager.GetRef().GuiSettings.PropertiesWidth;
      Height = SettingsManager.GetRef().GuiSettings.PropertiesHeight;
      cbPin.Checked = SettingsManager.GetRef().GuiSettings.PinProperties;
      if (!SettingsManager.GetRef().GuiSettings.PinProperties)
      {
        Left = SettingsManager.GetRef().GuiSettings.PropertiesLeft;
        Top = SettingsManager.GetRef().GuiSettings.PropertiesTop;
      }
    }

    public object CurrentObject
    {
      set
      {
        OnCurrentObjectChanged(value);
      }
    }

    private IEnumerable<KeyValuePair<string, string>> ConvertToString(object value)
    {
      if (value == null)
      {
        throw new ArgumentNullException("value");
      }

      var pole = value as Pole;

      if (pole != null)
      {
        yield return new KeyValuePair<string, string>("Id", pole.ID.ToString());
        yield return new KeyValuePair<string, string>("Type", pole.Type.ToString());
        yield return new KeyValuePair<string, string>("Position", ConvertToString(pole.Position));
        yield break;
      }

      var chromosome = value as Chromosome;

      if (chromosome != null)
      {
        yield return new KeyValuePair<string, string>("Id", chromosome.ID.ToString());
        yield return new KeyValuePair<string, string>("Position", ConvertToString(chromosome.Position));
        yield return new KeyValuePair<string, string>("Orientation", ConvertToString(
          Math.Atan2(chromosome.Orientation[3, 2], chromosome.Orientation[3, 3]),
          Math.Atan2(-chromosome.Orientation[3, 1], Math.Sqrt(Math.Pow(chromosome.Orientation[3, 2], 2) + Math.Pow(chromosome.Orientation[3, 3], 2))),
          Math.Atan2(chromosome.Orientation[2, 1], chromosome.Orientation[1, 1])));
        yield return new KeyValuePair<string, string>("Left pole MTs", chromosome.BoundMTs.Count(mt => mt.Pole.Type == PoleType.Left).ToString());
        yield return new KeyValuePair<string, string>("Right pole MTs", chromosome.BoundMTs.Count(mt => mt.Pole.Type == PoleType.Right).ToString());
        yield break;
      }

      var tube = value as MT;

      if (tube != null)
      {
        yield return new KeyValuePair<string, string>("Id", tube.ID.ToString());
        yield return new KeyValuePair<string, string>("Pole", tube.Pole.Type.ToString());
        yield return new KeyValuePair<string, string>("Length", tube.Length.ToString("E03"));
        yield return new KeyValuePair<string, string>("Direction", ConvertToString(tube.Direction));
        yield return new KeyValuePair<string, string>("State", String.Format("{0}, {1}",
                                                                             tube.State.ToString(),
                                                                             tube.BoundChromosome == null
                                                                                ? "free"
                                                                                : "bound"));
        yield break;
      }

      yield return new KeyValuePair<string, string>("Value", value.ToString());
    }

    private string ConvertToString(Vec3D vec3D)
    {
      return ConvertToString(vec3D.X, vec3D.Y, vec3D.Z);
    }

    private string ConvertToString(double x, double y, double z)
    {
      return string.Format("({0:E03}, {1:E03}, {2:E03})", x, y, z);
    }

    private void ClearRecords()
    {
      if (pnProperties.Controls.Count > 0)
      {
        Control[] controls = new Control[pnProperties.Controls.Count];
        pnProperties.Controls.CopyTo(controls, 0);
        pnProperties.Controls.Clear();
        foreach (var control in controls)
          control.Dispose();
      }
      _records = null;
    }

    private void SetNoSelection()
    {
      ClearRecords();

      Label lb = new Label();
      lb.Text = NoSelection;
      lb.Size = lb.PreferredSize;
      lb.Left = Delta;
      lb.Top = Delta;
      lb.Anchor = AnchorStyles.Left | AnchorStyles.Top;
      pnProperties.Controls.Add(lb);

      _hashCode = NoSelection.GetHashCode();
    }

    private void OnCurrentObjectChanged(object newObject)
    {
      if (newObject == null)
      {
        if (_hashCode == NoSelection.GetHashCode())
          return;

        SetNoSelection();
      }
      else
      {
        var texts = ConvertToString(newObject);

        //Checking hash code.
        {
          int newHashCode;
          StringBuilder sb = new StringBuilder();
          foreach (var text in texts)
          {
            sb.Append(text.Key);
            sb.Append("; ");
          }
          newHashCode = sb.ToString().GetHashCode();
          if (newHashCode != _hashCode)
            ClearRecords();
          _hashCode = newHashCode;
        }

        //Going to create new records.
        if (_records == null)
        {
          _records = new List<Record>();
          int curTop = Delta;
          int maxLabelRight = 0;

          //Generating and formatting new records.
          foreach (var propertyDesc in texts)
          {
            Label lb = new Label();
            lb.Text = propertyDesc.Key;
            lb.Size = lb.PreferredSize;
            lb.Top = curTop;
            lb.Left = Delta;
            lb.Anchor = AnchorStyles.Top | AnchorStyles.Left;
            if (lb.Right > maxLabelRight)
              maxLabelRight = lb.Right;

            TextBox tb = new TextBox();
            tb.ReadOnly = true;
            tb.Top = curTop;
            tb.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;

            curTop += Math.Max(lb.Height, tb.Height) + Delta;
            _records.Add(new Record(lb, tb));
          }

          //Aligning controls and adding them to panel.
          foreach (var record in _records)
          {
            record.TextBox.Left = maxLabelRight + Delta;
            record.TextBox.Width = pnProperties.Width - record.TextBox.Left - Delta;

            pnProperties.Controls.AddRange(new Control[] { record.Label, record.TextBox });
          }
        }

        //Assigning values to text boxes.
        for (int i = 0; i < _records.Count; i++)
        {
          _records[i].TextBox.Text = texts.ElementAt(i).Value;
        }
      }
    }

    private void cbPin_CheckedChanged(object sender, EventArgs e)
    { SettingsManager.GetRef().GuiSettings.PinProperties = cbPin.Checked; }

    private void SelectionPopupForm_FormClosing(object sender, FormClosingEventArgs e)
    {
      SettingsManager.GetRef().GuiSettings.PropertiesWidth = Width;
      SettingsManager.GetRef().GuiSettings.PropertiesHeight = Height;
      SettingsManager.GetRef().GuiSettings.PropertiesLeft = Left;
      SettingsManager.GetRef().GuiSettings.PropertiesTop = Top;

      _onClosing(sender, e);
    }
  }
}
