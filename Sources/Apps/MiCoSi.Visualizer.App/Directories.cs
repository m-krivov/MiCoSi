using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.IO;

namespace MiCoSi
{
  public class Directories
  {
    public static String GetHomeDirectory()
    {
      // Checking environment variables for manually set home directory
      try
      {
        String home = Environment.GetEnvironmentVariable("MITOSIS_HOME");
        if (home != null)
          return home;
      }
      catch (Exception) { }

      // Checking binary's location for installer created home directory
      try
      {
        String assembly = System.Reflection.Assembly.GetExecutingAssembly().Location;
        String home = (new FileInfo(assembly)).Directory.Parent.FullName;

        if (Directory.Exists(Path.Combine(home, "Bin")))
          return home;
      }
      catch (Exception) { }

      // Application was launched under VS, use the current directory
      return Path.Combine(Directory.GetCurrentDirectory(), "SDK");
    }

    public static String GetBinDirectory()
    {
      return System.IO.Path.Combine(GetHomeDirectory(), "Bin");
    }

    public static String GetDocDirectory()
    {
      return System.IO.Path.Combine(GetHomeDirectory(), "Docs");
    }

    public static String GetConfigDirectory()
    {
      return System.IO.Path.Combine(GetHomeDirectory(), "UI");
    }

    public static String GetResourceDirectory()
    {
      return System.IO.Path.Combine(GetHomeDirectory(), "UI");
    }

    public static String GetSettingsDirectory()
    {
      String home = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                                           "Mitosis");

      try
      {
        if (!System.IO.Directory.Exists(home))
          System.IO.Directory.CreateDirectory(home);
      }
      catch (Exception) { }

      return home;
    }

    public static String GetSettingsFile()
    {
      return System.IO.Path.Combine(GetSettingsDirectory(), "Settings.bin");
    }

    public static String GetTempDirectory()
    {
      return Environment.GetEnvironmentVariable("TEMP");
    }
  }
}
