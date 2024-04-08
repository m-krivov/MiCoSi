using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace MiCoSi
{
  ///<summary>
  /// Contains output and return code, that were revieved from simulator.
  ///</summary>
  public class LaunchResult
  {
    public String Output { get; private set; }
    public String Error { get; private set; }
    public int ExitCode { get; private set; }

    internal LaunchResult(String output,
                          String error,
                          int exitCode)
    {
      Output = output;
      Error = error;
      ExitCode = exitCode;
    }

    public bool ExitedWithError
    {
      get { return ExitCode != 0 || !String.IsNullOrEmpty(Error); }
    }

    public override String ToString()
    { return Output; }
  };

  public class MitosisFilenames
  {
    public String Config { get; private set; }
    public String InitialStates { get; private set; }
    public String PoleCoords { get; private set; }
    public String[] Cells { get; private set; }

    internal MitosisFilenames(String config, String initial, String poles, String[] cells)
    {
      Config = config;
      InitialStates = initial;
      PoleCoords = poles;
      Cells = cells.Clone() as String[];
    }
  }

  public class LaunchParameters : ICloneable
  {
    public CliArgs Args { get; set; }
    public SimParams Config { get; set; }
    public InitialStates InitialStates { get; set; }
    public PoleCoordinates PoleCoords { get; set; }

    public LaunchParameters()
    {
      Config = new SimParams();
      Args = new CliArgs();
      InitialStates = null;
      PoleCoords = null;
    }

    private String ExtractFilename(String workingDir, String filename)
    { return Path.IsPathRooted(filename) ? filename : Path.Combine(workingDir, filename); }

    internal MitosisFilenames ExtractFilenames(String workingDir)
    {
      String config = String.IsNullOrEmpty(Args.ConfigFile)
                ? null
                : ExtractFilename(workingDir, Args.ConfigFile);
      String initial = String.IsNullOrEmpty(Args.InitialConditionsFile)
                ? null
                : ExtractFilename(workingDir, Args.InitialConditionsFile);
      String poles = String.IsNullOrEmpty(Args.PoleCoordsFile)
                ? null
                : ExtractFilename(workingDir, Args.PoleCoordsFile);

      String[] cells = null;
      if (!String.IsNullOrEmpty(Args.CellFile) && Args.CellCount >= 1)
      {
        cells = CliArgs.MultiplyCells(Args.CellFile, Args.CellCount);
        for (int i = 0; i < cells.Length; i++)
          cells[i] = ExtractFilename(workingDir, cells[i]);
      }

      return new MitosisFilenames(config, initial, poles, cells);
    }

    internal String Apply(String workingDir)
    {
      if (Args == null) throw new ApplicationException("Cannot launch simulation without CLI arguments");

      CliArgs defaultArgs = new CliArgs();
      String oldCellFile = Args.CellFile;
      bool needCellFile = Args.Mode != LaunchMode.Help && Args.Mode != LaunchMode.Info;
      String oldConfig = Args.ConfigFile;
      bool needConfig = Args.Mode == LaunchMode.New || Args.Mode == LaunchMode.Continue || Args.Mode == LaunchMode.Restart;
      String oldInitialFile = Args.InitialConditionsFile;
      String oldPoleFile = Args.PoleCoordsFile;

      try
      {
        //Settings file for results.
        if (needCellFile || Args.CellFile != defaultArgs.CellFile)
          Args.CellFile = ExtractFilename(workingDir, oldCellFile);
        if (Args.Mode == LaunchMode.Fix)
          return Args.Export();

        //Storing config.
        if (needConfig || Args.ConfigFile != defaultArgs.ConfigFile)
        {
          if (Config == null) throw new ApplicationException("Cannot launch simulation without config");
          if (String.IsNullOrEmpty(Args.ConfigFile)) Args.ConfigFile = "mitosis.conf";
          Args.ConfigFile = ExtractFilename(workingDir, oldConfig);
          File.WriteAllText(Args.ConfigFile, Config.ExportAsProps());
        }

        //Storing initial states.
        if (InitialStates != null)
        {
          if (String.IsNullOrEmpty(Args.InitialConditionsFile)) Args.InitialConditionsFile = "initial.xml";
          Args.InitialConditionsFile = ExtractFilename(workingDir, Args.InitialConditionsFile);
          File.WriteAllText(Args.InitialConditionsFile, InitialStates.ToString());
        }

        //Storing information about poles.
        if (PoleCoords != null)
        {
          if (String.IsNullOrEmpty(Args.PoleCoordsFile)) Args.PoleCoordsFile = "poles.xml";
          Args.PoleCoordsFile = ExtractFilename(workingDir, Args.PoleCoordsFile);
          File.WriteAllText(Args.PoleCoordsFile, PoleCoords.ToString());
        }

        //Done, exporting.
        return Args.Export();
      }
      finally
      {
        Args.CellFile = oldCellFile;
        Args.ConfigFile = oldConfig;
        Args.InitialConditionsFile = oldInitialFile;
        Args.PoleCoordsFile = oldPoleFile;
      }
    }

    public object Clone()
    {
      var res = new LaunchParameters();

      if (Args != null) res.Args = Args.Clone() as CliArgs;
      if (Config != null) res.Config = Config.Clone() as SimParams;
      if (InitialStates != null) res.InitialStates = InitialStates.Clone() as InitialStates;
      if (PoleCoords != null) res.PoleCoords = PoleCoords.Clone() as PoleCoordinates;

      return res;
    }
  }

  ///<summary>
  /// Launches simulation process in separate process.
  ///</summary>
  public class Launcher : IDisposable
  {
    private String _simulatorFile;
    private System.Diagnostics.Process _proc;

    public String WorkingDir { get; private set; }

    private LaunchParameters _params;
    public LaunchParameters Params
    {
      get
      {
        if (_proc != null)
          throw new ApplicationException("Launcher parameters cannot be acccessed while simulation is not finished");
        return _params;
      }
      private set { _params = value; }
    }

    private void SafeRelease()
    {
      if (_proc != null)
        Terminate();
    }

    public Launcher(String workingDir, String simulatorFile, LaunchParameters parameters)
    {
      _proc = null;
      WorkingDir = workingDir;
      _simulatorFile = simulatorFile;
      _params = parameters.Clone() as LaunchParameters;
    }

    public Launcher(String workingDir, String simulatorFile)
      : this(workingDir, simulatorFile, new LaunchParameters())
    { /*nothing*/ }

    public void Start()
    {
      if (_proc != null)
        throw new ApplicationException("The simulation process has been already started");

      try
      {
        var args = _params.Apply(WorkingDir);
        _proc = new System.Diagnostics.Process();
        _proc.StartInfo.CreateNoWindow = true;
        _proc.StartInfo.RedirectStandardInput = true;
        _proc.StartInfo.RedirectStandardOutput = true;
        _proc.StartInfo.RedirectStandardError = true;
        _proc.StartInfo.UseShellExecute = false;
        _proc.StartInfo.FileName = _simulatorFile;
        _proc.StartInfo.Arguments = args;
        _proc.Start();
      }
      catch
      {
        _proc = null;
        throw;
      }
    }

    public LaunchResult Wait()
    {
      if (_proc == null)
        throw new ApplicationException("The simulation process is not started");
      String line;

      var sbOutput = new StringBuilder();
      while ((line = _proc.StandardOutput.ReadLine()) != null)
        sbOutput.AppendLine(line);

      var sbError = new StringBuilder();
      while ((line = _proc.StandardError.ReadLine()) != null)
        sbError.AppendLine(line);

      _proc.WaitForExit();
      int exitCode = _proc.ExitCode;
      _proc.Dispose();
      _proc = null;

      return new LaunchResult(sbOutput.ToString(),
                  sbError.ToString(),
                  exitCode);
    }

    public LaunchResult StartAndWait()
    {
      Start();
      return Wait();
    }

    public void Terminate()
    {
      if (_proc == null)
        throw new ApplicationException("The simulation process is not started");

      if (!_proc.HasExited)
        _proc.Kill();
      _proc.Dispose();
      _proc = null;
    }

    public void Dispose()
    { SafeRelease(); }

    ~Launcher()
    { SafeRelease(); }
  };
}
