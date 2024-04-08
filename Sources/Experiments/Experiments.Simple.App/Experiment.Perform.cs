using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Text;
using System.IO;
using System.Diagnostics;
using MiCoSi;

namespace Experiments
{
  public partial class Experiment
  {
    private static readonly String Tab                    = "   ";
    private static readonly String ProgramHash            = "Program.hash";
    private static readonly String SimulationHash         = "Simulation.hash";
    private static readonly String SimulationFlag         = "Done.flag";
    private static readonly String SimulationTemplate     = "{0}_{1}";          // like '0_SomeTest'
    private static readonly String AnalyzedCellDir        = "acells";
    private static readonly String AnalyzedCellTemplate   = "cell_{0}.acell";   // like 'cell_0.acell'
    private static readonly String CsvReportDir           = "reports";

    // Virtual experiments take a loot of time, hours and days
    // If possible, it is better to continue the previous experiment than to start a new one
    private void PrepareWorkingDirectory(String workingDir, Specification[] specs)
    {
      var hashPath = Path.Combine(workingDir, ProgramHash);

      bool canContinue = true;
      if (canContinue && !Directory.Exists(workingDir))
      {
        canContinue = false;
        Logging.WriteLine(Tab + "No previous simulations found, going to start new experiments");
      }

      // Ensure that they used the same version of the program
      if (canContinue &&
          (!File.Exists(hashPath) || File.ReadAllText(hashPath) != Hash()))
      {
        canContinue = false;
        Logging.WriteLine(Tab + "Program version changed, cannot continue the previous simulations");
      }

      // If something has changed, restart the whole experiment
      if (!canContinue)
      {
        if (Directory.Exists(workingDir))
        { Directory.Delete(workingDir, true); }

        Directory.CreateDirectory(workingDir);
        File.WriteAllText(hashPath, Hash());
      }

      // Otherwise, just delete unknown files and subdirectories
      else
      {
        var files = Directory.GetFiles(workingDir);
        foreach (var f in files)
        {
          if (new FileInfo(f).FullName != new FileInfo(hashPath).FullName)
          { File.Delete(f); }
        }

        var simulationDirs = new HashSet<String>();
        for (int i = 0; i < specs.Length; i++)
        {
          var dir = Path.Combine(workingDir, String.Format(SimulationTemplate, i, specs[i].Name));
          simulationDirs.Add(new DirectoryInfo(dir).FullName);
        }

        var dirs = Directory.GetDirectories(workingDir);
        foreach (var dir in dirs)
        {
          if (!simulationDirs.Contains(new DirectoryInfo(dir).FullName))
          { Directory.Delete(dir, true); }
        }

        Logging.WriteLine(Tab + "Previous simulations found, going to continue them");
      } // if canContinue
    }

    /// <summary>
    /// Simulates the requested number of cells, returns a list of the resulting *.cell files
    /// </summary>
    private String[] SimulateBatch(String simDir, uint[] seeds, Specification spec, Stopwatch solverTime)
    {
      var parameters = new LaunchParameters();
      parameters.Config = spec.Config.Clone() as SimParams;

      // Create initial state of the cell
      parameters.InitialStates = new InitialStates();
      var N_MT_Total = parameters.Config[SimParameter.Int.N_MT_Total];
      Random rnd = new Random((int)seeds[0]);
      for (int i = 0; i < N_MT_Total * 2; i++)
      {
        double alpha = (rnd.NextDouble()) * Math.PI * 2;
        double dx = rnd.NextDouble() * (i < N_MT_Total ? 1 : -1);
        double dy = Math.Sqrt(1.0 - dx * dx) * Math.Cos(alpha);
        double dz = Math.Sqrt(1.0 - dx * dx) * Math.Sin(alpha);
        parameters.InitialStates.AddMT(i < N_MT_Total ? PoleType.Left : PoleType.Right,
                                       dx, dy, dz, 0.0, MTState.Depolymerization);
      }
      if (spec.Chromosomes != null)
      {
        foreach (var chr in spec.Chromosomes)
        {
          var pos = chr.Item1;
          var ori = chr.Item2;
          parameters.InitialStates.AddChromosomePair(pos.X, pos.Y, pos.Z,
                                                     ori.X, ori.Y, ori.Z);
        }
      }

      // Define how to move poles
      parameters.PoleCoords = null;
      if (spec.MovePoles)
      {
        parameters.PoleCoords = new PoleCoordinates();
        double t = 180.0;
        double pole_r = parameters.Config[SimParameter.Double.L_Poles] / 2 * 1e-6;
        for (int i = 0; i < t; i++)
        {
          double angle = i / (t - 1) * Math.PI / 2;
          parameters.PoleCoords.AddRecord(i, -pole_r * Math.Sin(angle), pole_r * Math.Cos(angle), 0.0,
                                          pole_r * Math.Sin(angle), pole_r * Math.Cos(angle), 0.0);
        }
      }

      // Simulate the requested cells using an external solver
      parameters.Args.Mode = LaunchMode.New;
      parameters.Args.Solver = new SimulatorConfig(SimulatorType.CPU);
      parameters.Args.CellCount = seeds.Length;
      parameters.Args.CellFile = "results.cell";
      parameters.Args.UserSeed = seeds[0];    // fix me: transfer all seeds to solver

      using (Launcher ml = new Launcher(simDir, _solverPath, parameters))
      {
        solverTime.Start();
        var ret = ml.StartAndWait();
        solverTime.Stop();

        if (ret.ExitCode != 0 || !String.IsNullOrEmpty(ret.Error))
        {
          throw new ApplicationException(String.Format("Simulation failed, error #{0}: {1}, {2}",
                                                       ret.ExitCode, ret.Error, ret.Output));
        }
      }

      // Prepare and return the list with cell files
      String[] res = new String[seeds.Length];
      var defaultFile = new FileInfo(Path.Combine(simDir, parameters.Args.CellFile)).FullName;
      if (res.Length == 1)
      { res[0] = defaultFile; }
      else
      {
        for (int i = 0; i < res.Length; i++)
        {
          res[i] = defaultFile.Replace("results.cell", String.Format("results_{0}.cell", i));
          if (!File.Exists(res[0]))
          {
            throw new ApplicationException(
                String.Format("File with the raw simulation data was not created: '{0}'", res[0])
            );
          }
        } // for i
      } // else

      return res;
    }
    
    /// <summary>
    /// Performs or continues a single simulation
    /// If results are already presented, does nothing
    /// </summary>
    private void PerformSimulation(IAnalyzer[] analyzers, String simDir, Specification spec,
                                   Random rng, Stopwatch solverTime)
    {
      // Ensure that the directory is valid
      bool canContinue = true;
      String hashPath = Path.Combine(simDir, SimulationHash);
      if (!File.Exists(hashPath) ||
          File.ReadAllText(hashPath) != spec.Hash())
      { canContinue = false; }

      if (!canContinue)
      {
        if (Directory.Exists(simDir))
        { Directory.Delete(simDir, true); }
        Directory.CreateDirectory(simDir);

        File.WriteAllText(hashPath, spec.Hash());
      }

      // Check the 'done' flag
      String doneFlag = Path.Combine(simDir, SimulationFlag);
      if (File.Exists(doneFlag))
      {
        Logging.WriteLine(Tab + "Already done, skipping");
        return;
      }

      // Generate list of the required seeds, load the previously analyzed cells
      uint[] seeds = new uint[spec.Cells];
      for (int i = 0; i < seeds.Length; i++)
      { seeds[i] = (uint)rng.Next(); }

      var acellDir = Path.Combine(simDir, AnalyzedCellDir);
      if (!Directory.Exists(acellDir))
      { Directory.CreateDirectory(acellDir); }

      var acells = new List<AnalyzedCell>();
      {
        int i = 0;
        String acellPath;
        while (File.Exists(acellPath = Path.Combine(acellDir, String.Format(AnalyzedCellTemplate, i))))
        {
          AnalyzedCell acell = null;
          if (AnalyzedCell.TryLoad(acellPath, out acell) &&
              acell.Seed == seeds[i])
          { acells.Add(acell); }
          else
          { break; }

          i += 1;
        }
        if (acells.Count > 0)
        { Logging.WriteLine(Tab + String.Format("{0} cell(s) have been analyzed previously", acells.Count)); }
      }

      // Perform computations and analysis, store intermidiate results
      while (acells.Count < seeds.Length)
      {
        var firstCellIdx = acells.Count;
        var cellCount = Math.Min(BatchSize, seeds.Length - acells.Count);
        
        // Say greetings
        if (cellCount == 1)
        { Console.WriteLine(Tab + "Cell #" + (firstCellIdx + 1).ToString()); }
        else
        {
          Console.WriteLine(String.Format("{0}Cells #{1}...#{2}", Tab,
                                          firstCellIdx + 1, firstCellIdx + cellCount));
        }

        // The simulation itself
        uint[] batchSeeds = new uint[cellCount];
        Array.Copy(seeds, firstCellIdx, batchSeeds, 0, cellCount);
        var files = SimulateBatch(simDir, batchSeeds, spec, solverTime);        // in parallel

        // Analyze the raw data
        for (int i = 0; i < cellCount; i++)
        { acells.Add(new AnalyzedCell(batchSeeds[i])); }

        Parallel.For(0, cellCount, OurParallelOptions, (cellIdx) =>
        {
          using (var ts = TimeStream.Open(files[cellIdx]))
          {
            double[] timeTable = new double[ts.LayerCount];
            for (int i = 0; i < ts.LayerCount; i++)
            { timeTable[i] = ts.GetLayerTime(i); }

            var acell = acells[firstCellIdx + (int)cellIdx];
            acell.CreateTimeTable(timeTable);

            foreach (var a in analyzers)
            { a.Initialize(acell, spec.Config, ts.LayerCount); }

            ts.Reset();
            int layer = 0;
            while (ts.MoveNext())
            {
              foreach (var a in analyzers)
              { a.Process(acell, spec.Config, ts.Current.Cell, layer); }
              layer += 1;
            }

            var acellPath = Path.Combine(simDir, AnalyzedCellDir,
                                         String.Format(AnalyzedCellTemplate, firstCellIdx + cellIdx));
            acell.Save(acellPath);
          } // using ts
        }); // parallel for cell

        // Some cleaning up: delete the excessive *.cell files to save disk space
        for (int i = 1; i < files.Length; i++)
        { File.Delete(files[i]); }

      } // while

      // Format and store results, some other clean up
      var reportDir = Path.Combine(simDir, CsvReportDir);
      if (Directory.Exists(reportDir))
      { Directory.Delete(reportDir, true); }
      Directory.CreateDirectory(reportDir);

      var cells = acells.ToArray();
      for (int i = 0; i < analyzers.Length; i++)
      { analyzers[i].Format(cells, spec.Config, reportDir, (uint)i); }

      var toDelete = Directory.GetFiles(Path.Combine(simDir, AnalyzedCellDir),
                                        String.Format(AnalyzedCellTemplate, "*"));
      foreach (var file in toDelete)
      { File.Delete(file); }

      File.WriteAllText(doneFlag, "");
    }

    /// <summary>
    /// Simulates the requsted scenarios
    /// Uses the 'workingDir' directory to store an intermediate results and the final tables
    /// </summary>
    public void Perform(String workingDir, IEnumerable<Specification> setups)
    {
      var specs = setups.ToArray();
      Logging.WriteLine(String.Format("Going to perform {0} simulation(s)", specs.Length));
      PrepareWorkingDirectory(workingDir, specs);
      Logging.WriteLine();

      var rnd = new Random(Seed);
      var analyzers = CreateAnalyzers();
      if (analyzers.Length == 0)
      {
        Logging.WriteLine("No analyzers specified, aborting");
        return;
      }

      Stopwatch swGlobal = new Stopwatch(), swLocal = new Stopwatch(), swSolver = new Stopwatch();
      swGlobal.Start();
      for (int i = 0; i < specs.Length; i++)
      {
        Logging.WriteLine(String.Format("Simulation '{0}'", specs[i].Name));
        var simDir = Path.Combine(workingDir, String.Format(SimulationTemplate, i, specs[i].Name));

        swLocal.Restart();
        PerformSimulation(analyzers, simDir, specs[i], new Random(rnd.Next()), swSolver);
        swLocal.Stop();

        long localTime = swLocal.ElapsedMilliseconds, solverTime = swSolver.ElapsedMilliseconds;
        swSolver.Reset();
        Logging.WriteLine(String.Format("Completed in {0} seconds", localTime / 1000));
        Logging.WriteLine(String.Format(System.Globalization.CultureInfo.InvariantCulture,
                                        "{0}Simulation: {1}% ({2} seconds)", Tab,
                                        ((1000 * solverTime) / Math.Max(localTime, 1)) * 0.1,
                                        solverTime / 1000));
        Logging.WriteLine(String.Format(System.Globalization.CultureInfo.InvariantCulture,
                                        "{0}Analysis:   {1}% ({2} seconds)", Tab,
                                        ((1000 * (localTime - solverTime)) / Math.Max(localTime, 1)) * 0.1,
                                        (localTime - solverTime) / 1000));
        Logging.WriteLine();
      }
      swGlobal.Stop();

      Logging.WriteLine(String.Format("All {0} simulation(s) are completed in {1} seconds",
                                      specs.Length, swGlobal.ElapsedMilliseconds / 1000));
      Logging.WriteLine(String.Format("Results are stored to '{0}'", new FileInfo(workingDir).FullName));
    }
  }
}
