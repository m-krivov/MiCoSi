using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Mitosis
{
  static class Program
  {
    /// <summary>
    /// Handler for assembly searching in home directory
    /// </summary>
    private static System.Reflection.Assembly ResolveHandler(object sender, ResolveEventArgs args)
    {
      String homeDir = Environment.GetEnvironmentVariable("MITOSIS_HOME");
      if (homeDir == null)
        return null;

      String fullPath = System.IO.Path.Combine(System.IO.Path.Combine(homeDir, "Bin"),
                                               args.Name.Substring(0, args.Name.IndexOf(",")) + ".dll");
      if (!System.IO.File.Exists(fullPath))
        return null;

      return System.Reflection.Assembly.LoadFrom(fullPath);
    }

    /// <summary>
    /// The main entry point for the application
    /// </summary>
    [STAThread]
    static void Main(String[] args)
    {
      try
      { SettingsManager.GetRef().Load(); }
      catch (Exception)
      {
        UserErrors.ConfigFileCorrupted("File with user settings", "using default settings");
        SettingsManager.GetRef().Reset();
      }

      try
      {
        System.AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(ResolveHandler);
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        if (args.Length == 1)
        {
          if (args[0] == "--auto_start")
          {
            if (System.IO.File.Exists(SettingsManager.GetRef().GuiSettings.DefaultCellFile))
              Application.Run(new MainForm(SettingsManager.GetRef().GuiSettings.DefaultCellFile));
            else
              Application.Run(new MainForm());
          }
          else
            Application.Run(new MainForm(args[0]));
        }
        else
          Application.Run(new MainForm());
      }
      catch (Exception ex)
      {
        MessageBox.Show(String.Format("Global error has been detected. Program will be closed{0}Details: {1}",
                                      Environment.NewLine, ex.ToString()),
                        "Global error");
        Environment.Exit(0);
      }
    }
  }
}
