using System;
using System.Windows.Forms;

namespace Mitosis
{
  public partial class MainForm : Form
  {
    private delegate void GuiAction(Object obj);
    private class FailedToLoadSimResultsException : ApplicationException
    {
      public FailedToLoadSimResultsException(string message)
        : base(message)
      { }
    }

    private MainFormContext _context;

    private void PerformAction(GuiAction action, Object arg, bool pausePlayback)
    {
      bool timerEnabled = false;
      if (timer != null)
      {
        timerEnabled = timer.Enabled;
        timer.Enabled = false;
      }

      bool playbackPaused = false;
      if (_context != null && pausePlayback)
      {
        playbackPaused = _context.Paused;
        _context.Paused = true;
      }

      try
      {
        action(arg);
      }
      catch (VersionConflictException ex)
      { UserErrors.VersionConflict(ex.RequiredVersion); }
      catch (FailedToLoadSimResultsException ex)
      { UserErrors.FailedToLoadSimulationResults(ex.Message); }
      catch (ApplicationException ex)
      { UserErrors.UnhandledException(ex, true); }
      finally
      {
        if (_context != null && pausePlayback)
          _context.Paused = playbackPaused;
        if (timer != null)
          timer.Enabled = timerEnabled;
      }

    }

    private void Action_Open(Object arg)
    {
      if (_context != null)
      {
        if (_context.SimResultsLoaded)
          _context.Close();
        try
        {
          _context.Open(arg as String);
        }
        catch (Exception ex)
        {
          if (_context.SimResultsLoaded)
            _context.Close();

          if (ex.GetType() == typeof(VersionConflictException))
            throw;
          else
            throw new FailedToLoadSimResultsException(ex.Message);
        }
      }
    }

    private bool Open(String cellFile)
    {
      PerformAction(Action_Open, cellFile, true);
      return _context != null && _context.SimResultsLoaded;
    }

    public MainForm()
    {
      InitializeComponent();
      Top = SettingsManager.GetRef().GuiSettings.WindowTop;
      Left = SettingsManager.GetRef().GuiSettings.WindowLeft;
      Width = SettingsManager.GetRef().GuiSettings.WindowWidth;
      Height = SettingsManager.GetRef().GuiSettings.WindowHeight;

      _context = new MainFormContext(this, pRenderArea, tbTime, lbTime,
                                     btStartStop, nudSpeed,
                                     btScreenshot, btVideoCapturing, btInfo);
      timer.Enabled = true;
    }

    public MainForm(String cellFile)
      :this()
    {
      if (cellFile != null)
        Open(cellFile);
    }

    private void tsmiExit_Click(object sender, EventArgs e)
    { Close(); }

    private void Action_Close(Object arg)
    {
      if (_context != null && _context.SimResultsLoaded)
        _context.Close();
    }

    private void tsmiClose_Click(object sender, EventArgs e)
    { PerformAction(Action_Close, null, false); }

    private void Action_GuiOpen(Object arg)
    {
      //Getting working directory.
      OpenFileDialog ofd = new OpenFileDialog();
      ofd.Filter = "Mitosis Files (*.cell)|*.cell|All Files|*.*";
      if (System.IO.Directory.Exists(SettingsManager.GetRef().GuiSettings.DefaultCellFile))
        ofd.InitialDirectory = (new System.IO.FileInfo(SettingsManager.GetRef().GuiSettings.DefaultCellFile)).DirectoryName;
      
      var result = ofd.ShowDialog();
      if (result == DialogResult.OK || result == DialogResult.Yes)
      {
        if (Open(ofd.FileName))
          SettingsManager.GetRef().GuiSettings.DefaultCellFile = ofd.FileName;
      }
    }

    private void tsmiOpen_Click(object sender, EventArgs e)
    { PerformAction(Action_GuiOpen, null, true); }

    private void Action_ChangeVisualSettings(Object arg)
    {
      if (_context != null &&
        VisualSettingsForm.CreateAndShow(Mitosis.DxRender.StyleManager.GetKnownNames(),
                                         SettingsManager.GetRef().RenderSettings))
      { _context.Recreate(true); }
    }

    private void tsmiVisual_Click(object sender, EventArgs e)
    { PerformAction(Action_ChangeVisualSettings, null, true); }

    private void Action_ChangeCaptureSettings(Object arg)
    {
      if (_context != null &&
        CaptureSettingsForm.CreateAndShow(CaptureOptions.ImageSizes,
                                          CaptureOptions.ImageContainers,
                                          CaptureOptions.VideoCompressions,
                                          CaptureOptions.VideoSizes,
                                          CaptureOptions.VideoFPSs,
                                          SettingsManager.GetRef().CaptureSettings))
      { _context.Recreate(true); }
    }

    private void tsmiCapture_Click(object sender, EventArgs e)
    { PerformAction(Action_ChangeCaptureSettings, null, true); }

    private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
    {
      try
      { _context.Dispose(); }
      catch (Exception) { }

      SettingsManager.GetRef().GuiSettings.WindowTop = Top;
      SettingsManager.GetRef().GuiSettings.WindowLeft = Left;
      SettingsManager.GetRef().GuiSettings.WindowWidth = Width;
      SettingsManager.GetRef().GuiSettings.WindowHeight = Height;
      SettingsManager.GetRef().GuiSettings.Speed = nudSpeed.Value;

      try
      { SettingsManager.GetRef().Save(); }
      catch (Exception) { }
    }

    private void Action_Resize(Object arg)
    {
      if (_context != null)
        _context.Recreate(false);
    }

    private void pRenderArea_Resize(object sender, EventArgs e)
    {
      if (WindowState == FormWindowState.Minimized)
        return;
      PerformAction(Action_Resize, null, false);
    }

    private void Action_OnTick(Object arg)
    {
      if (_context != null)
        _context.OnTick();
    }

    private void timer_Tick(object sender, EventArgs e)
    { PerformAction(Action_OnTick, null, false); }

    private void tsmiAbout_Action(Object arg)
    { (new AboutForm()).ShowDialog(); }

    private void tsmiAbout_Click(object sender, EventArgs e)
    { PerformAction(tsmiAbout_Action, null, true); }

    private void tsmiHowDoI_Click(object sender, EventArgs e)
    {
      String helpPath = System.IO.Path.Combine(Directories.GetDocDirectory(), "Help.pdf");
      if (!System.IO.File.Exists(helpPath))
        UserErrors.BadInstallation("documentation", false);
      else
        System.Diagnostics.Process.Start("explorer", helpPath);
    }
  }
}
