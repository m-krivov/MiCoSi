using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MiCoSi
{
  public partial class CaptureSettingsForm : Form
  {
    private bool _shouldApply;

    private CaptureSettingsForm(List<String> imageSizes, int selectedImageSize,
                                List<String> imageContainers, int selectedImageContainer,
                                List<String> videoCompressions, int selectedVideoCompression,
                                List<String> videoSizes, int selectedVideoSize,
                                List<String> videoFPS, int selectedVideoFPS,
                                String namePrefix, String outDirectory)
    {
      InitializeComponent();

      _shouldApply = false;

      cbImageSize.Items.AddRange(imageSizes.ToArray());
      cbImageSize.SelectedIndex = selectedImageSize;

      cbImageContainer.Items.AddRange(imageContainers.ToArray());
      cbImageContainer.SelectedIndex = selectedImageContainer;

      cbVideoCompression.Items.AddRange(videoCompressions.ToArray());
      cbVideoCompression.SelectedIndex = selectedVideoCompression;

      cbVideoSize.Items.AddRange(videoSizes.ToArray());
      cbVideoSize.SelectedIndex = selectedVideoSize;

      cbVideoFPS.Items.AddRange(videoFPS.ToArray());
      cbVideoFPS.SelectedIndex = selectedVideoFPS;

      tbNamePrefix.Text = namePrefix;
      tbOutFolder.Text = outDirectory;
    }

    static private void GenerateItems<T>(IEnumerable<T> items, T selectedItem,
                                         out List<String> names, out int selectedIndex) where T : struct, IComparable
    {
      names = new List<String>();

      selectedIndex = 0;
      int i = 0;
      foreach (T item in items)
      {
        if (item is CaptureOptions.ImageSize)
          names.Add(CaptureOptions.ToString((CaptureOptions.ImageSize)(Object)item));
        else if (item is CaptureOptions.ImageContainer)
          names.Add(CaptureOptions.ToString((CaptureOptions.ImageContainer)(Object)item));
        else if (item is CaptureOptions.VideoCompression)
          names.Add(CaptureOptions.ToString((CaptureOptions.VideoCompression)(Object)item));
        else if (item is CaptureOptions.VideoFPS)
          names.Add(CaptureOptions.ToString((CaptureOptions.VideoFPS)(Object)item));
        else if (item is CaptureOptions.VideoSize)
          names.Add(CaptureOptions.ToString((CaptureOptions.VideoSize)(Object)item));
        else
          throw new ApplicationException("Internal error - unknown enumeration");

        if (item.CompareTo(selectedItem) == 0)
          selectedIndex = i;
        i += 1;
      }
    }

    static internal bool CreateAndShow(IEnumerable<CaptureOptions.ImageSize> imageSizes,
                                       IEnumerable<CaptureOptions.ImageContainer> imageContainers,
                                       IEnumerable<CaptureOptions.VideoCompression> videoCompressions,
                                       IEnumerable<CaptureOptions.VideoSize> videoSizes,
                                       IEnumerable<CaptureOptions.VideoFPS> videoFPSs,
                                       CaptureSettings settings)
    {
      List<String> _imageSizes, _imageContainers, _videoCompressions, _videoSizes, _videoFPSs;
      int _selectedImageSize, _selectedImageContainer, _selectedVideoCompression,
        _selectedVideoSize, _selectedVideoFPS;
      GenerateItems<CaptureOptions.ImageSize>(imageSizes, settings.ImageSize,
                                              out _imageSizes, out _selectedImageSize);
      GenerateItems<CaptureOptions.ImageContainer>(imageContainers, settings.ImageContainer,
                                                   out _imageContainers, out _selectedImageContainer);
      GenerateItems<CaptureOptions.VideoCompression>(videoCompressions, settings.VideoCompression,
                                                     out _videoCompressions, out _selectedVideoCompression);
      GenerateItems<CaptureOptions.VideoSize>(videoSizes, settings.VideoSize,
                                              out _videoSizes, out _selectedVideoSize);
      GenerateItems<CaptureOptions.VideoFPS>(videoFPSs, settings.VideoFPS,
                                             out _videoFPSs, out _selectedVideoFPS);

      CaptureSettingsForm form = new CaptureSettingsForm(_imageSizes, _selectedImageSize,
                                                         _imageContainers, _selectedImageContainer,
                                                         _videoCompressions, _selectedVideoCompression,
                                                         _videoSizes, _selectedVideoSize,
                                                         _videoFPSs, _selectedVideoFPS,
                                                         settings.NamePrefix, settings.OutDirectory);

      form.ShowDialog();

      if (form._shouldApply)
      {
        settings.ImageSize
          = new List<CaptureOptions.ImageSize>(imageSizes)[Math.Max(0, form.cbImageSize.SelectedIndex)];
        settings.ImageContainer
          = new List<CaptureOptions.ImageContainer>(imageContainers)[Math.Max(0, form.cbImageContainer.SelectedIndex)];
        settings.VideoCompression
          = new List<CaptureOptions.VideoCompression>(videoCompressions)[Math.Max(0, form.cbVideoCompression.SelectedIndex)];
        settings.VideoSize
          = new List<CaptureOptions.VideoSize>(videoSizes)[Math.Max(0, form.cbVideoSize.SelectedIndex)];
        settings.VideoFPS
          = new List<CaptureOptions.VideoFPS>(videoFPSs)[Math.Max(0, form.cbVideoFPS.SelectedIndex)];
        settings.NamePrefix = form.tbNamePrefix.Text;
        settings.OutDirectory = form.tbOutFolder.Text;
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

    private void btSelectFolder_Click(object sender, EventArgs e)
    {
      FolderBrowserDialog fbd = new FolderBrowserDialog();
      if (System.IO.Directory.Exists(tbOutFolder.Text))
      { fbd.SelectedPath = tbOutFolder.Text; }
      else
      { fbd.SelectedPath = "."; }

      var result = fbd.ShowDialog();
      if (result == DialogResult.OK || result == DialogResult.Yes)
      { tbOutFolder.Text = fbd.SelectedPath; }
    }
  }
}
