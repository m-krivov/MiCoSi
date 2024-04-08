using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace MiCoSi
{
  public static class UserErrors
  {
    public static void ConfigFileCorrupted(String fileDescription, String action)
    {
      MessageBox.Show(String.Format("{0} is corrupted, {1}.", fileDescription, action),
                      "Local error");
    }

    public static void SettingsFileCorrupted(String action)
    {
      ConfigFileCorrupted("Settings file", action);
    }

    public static void VersionConflict(String requiredVersion)
    {
      MessageBox.Show(String.Format("Cannot open simulation results due to version conflict. Required version is '{0}'",
                                    requiredVersion),
                                    "Local error");
    }

    public static void CompilationConflict(String requiredFlags)
    {
      MessageBox.Show(String.Format("Cannot open simulation results because it was prepared" +
                                    " with different compilation flags. Required flags are '{0}'",
                                    requiredFlags),
                                    "Local error");
    }

    public static void FailedToLoadSimulationResults(String message)
    {
      MessageBox.Show(String.Format("Failed to open simulation results.{0}Details: {1}",
                      Environment.NewLine, message),
                      "Local error");
    }

    public static void BadInstallation(String missedFile, bool isFatal)
    {
      if (isFatal)
      {
        MessageBox.Show(String.Format("Failed to open {0} files, try to reinstall application", missedFile),
                        "Fatal error");
        System.Environment.Exit(0);
      }
      else
      {
        MessageBox.Show(String.Format("Failed to open {0} files, try to reinstall application", missedFile),
                        "Local error");
      }
    }

    public static void UnhandledException(Exception ex, bool isFatal)
    {
      if (isFatal)
      {
        MessageBox.Show(
          String.Format("Internal error has been detected, mitosis visualizer will be closed. " +
                        "Please, send the following information to developers:{0}{1}",
                        Environment.NewLine,
                        ex.ToString()),
          "Fatal error");
        System.Environment.Exit(0);
      }
      else
      {
        MessageBox.Show(
          String.Format("Internal error has been detected. " +
                        "Please, send the following information to developers:{0}{1}",
                        Environment.NewLine,
                        ex.ToString()),
          "Local error");
      }
    }
  }
}
