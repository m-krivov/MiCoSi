using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Globalization;
using System.Threading.Tasks;
using System.Diagnostics;
using MiCoSi;

namespace Experiments
{
  internal class Experiment
  {
    public String Name { get; set; }
    public SimParams Config { get; set; }
    public InitialStates States { get; set; }
    public PoleCoordinates Poles { get; set; }

    public Experiment()
    {
      Name = "Default";
      Config = new SimParams();
      States = new InitialStates();
      Poles = new PoleCoordinates();
    }

    public void Initialize(Random rnd, bool movePoles, bool hasLength)
    {
      var L_Poles = Config[SimParameter.Double.L_Poles];
      var R_Cell = Config[SimParameter.Double.R_Cell];
      var N_MT_Total = Config[SimParameter.Int.N_MT_Total];

      // MTs
      States = new InitialStates();
      for (int i = 0; i < N_MT_Total * 2; i++)
      {
        double alpha = (rnd.NextDouble()) * Math.PI * 2;
        double dx = rnd.NextDouble() * (i < N_MT_Total ? 1 : -1);
        double dy = Math.Sqrt(1.0 - dx * dx) * Math.Cos(alpha);
        double dz = Math.Sqrt(1.0 - dx * dx) * Math.Sin(alpha);
        if (hasLength)
        {
          double a = i < N_MT_Total ? -L_Poles / 2 : L_Poles / 2;
          double t = 0.0;
          if (movePoles)
          { throw new NotImplementedException("moving poles with length"); }
          else
          { t = -dx * a + Math.Sqrt(a * a * (dx * dx - 1) + R_Cell * R_Cell); }
          double max_l = Math.Sqrt((dx * t) * (dx * t) + (dy * t) * (dy * t) + (dz * t) * (dz * t));
          States.AddMT(i < N_MT_Total ? PoleType.Left : PoleType.Right,
                       dx, dy, dz,
                       rnd.NextDouble() * max_l * 1e-6,
                       (rnd.Next() % 2 == 0) ? MTState.Polymerization : MTState.Depolymerization);
        }
        else
        {
          States.AddMT(i < N_MT_Total ? PoleType.Left : PoleType.Right,
                     dx, dy, dz, 0.0, MTState.Depolymerization);
        }
      }

      // Poles
      if (movePoles)
      {
        Poles = new PoleCoordinates();
        double t = 180.0;
        double pole_r = L_Poles / 2 * 1e-6;
        for (int i = 0; i < t; i++)
        {
          double angle = i / (t - 1) * Math.PI / 2;
          Poles.AddRecord(i,
                          -pole_r * Math.Sin(angle), pole_r * Math.Cos(angle), 0.0,
                          pole_r * Math.Sin(angle), pole_r * Math.Cos(angle), 0.0);
        }
      }
      else
      { Poles = null; }
    }

  }

  class Program
  {
    private static char csv_separator = ';';        // change to ';' for the Russian version of MS Excel
    private static int cellsPerCase = 2;            // the size of cell ensemble
    private static bool hasDiff   = true;           // enable diffusion

    private static void Simulate(Experiment exp, Random rnd)
    {
      var testID = exp.Name;
      var config = exp.Config;

      // Prepare directory
      if (Directory.Exists(testID))
      { Directory.Delete(testID, true); }
      Directory.CreateDirectory(testID);
      Directory.SetCurrentDirectory(testID);

      // Cache some values that we are goting to use for configuration
      double R_Cell = config[SimParameter.Double.R_Cell];
      double L_Poles = config[SimParameter.Double.L_Poles];
      int N_MT_Total = config[SimParameter.Int.N_MT_Total];
      double Dt = config[SimParameter.Double.Dt];
      double Save_Freq_Macro = config[SimParameter.Double.Save_Freq_Macro];
      double T_End = config[SimParameter.Double.T_End];
      int layerCount = (int)(T_End / Save_Freq_Macro) + 1;

      // Ready to start
      int[] res_mts = new int[layerCount * 4 * cellsPerCase];
      double[] res_angles = new double[layerCount * 2 * cellsPerCase];
      double[] res_centromere = new double[layerCount * cellsPerCase];
      Stopwatch sw = new Stopwatch();

      int batchSize = Math.Max(1, Environment.ProcessorCount);
      int simN = 0;
      while (simN < cellsPerCase)
      {
        var parameters = new LaunchParameters();
        var cellCount = Math.Min(batchSize, cellsPerCase - simN);
        sw.Reset();

        // Say greetings
        if (cellCount == 1)
        { Console.WriteLine("Cell #" + (simN + 1).ToString()); }
        else
        { Console.WriteLine(String.Format("Cells #{0}...#{1}", simN + 1, simN + cellCount)); }
        Console.Write("   Simulating ... ");

        // Update parameters in accordance with our experiment
        sw.Start();
        parameters.Config = config;
        config[SimParameter.Double.D_Trans] = hasDiff ? SimParams.GetDefaultValue(SimParameter.Double.D_Trans, false) : 0.0;
        config[SimParameter.Double.D_Rot] = hasDiff ? SimParams.GetDefaultValue(SimParameter.Double.D_Rot, false) : 0.0;
        parameters.InitialStates = exp.States;
        parameters.PoleCoords = exp.Poles;

        // Simulate
        parameters.Args.Mode = LaunchMode.New;
        parameters.Args.Solver = new SimulatorConfig(SimulatorType.CPU);
        parameters.Args.CellCount = cellCount;
        parameters.Args.CellFile = "results.cell";
        using (Launcher ml = new Launcher(".", "../Mitosis.exe", parameters))
        {
          var res = ml.StartAndWait();
          if (res.ExitCode != 0 || !String.IsNullOrEmpty(res.Error))
          {
            throw new ApplicationException(String.Format("Simulation failed, error #{0}: {1}, {2}",
                                                         res.ExitCode, res.Error, res.Output));
          }
        }
        sw.Stop();
        Console.WriteLine(String.Format("{0} seconds", (int)sw.ElapsedMilliseconds / 1000));

        // Analyze
        Console.Write("   Counting ... ");
        sw.Reset();
        sw.Start();
        var pfor = Parallel.For(simN, simN + cellCount, cellN =>
        {
          var ts = TimeStream.Open(cellCount == 1
                                  ? parameters.Args.CellFile
                                  : parameters.Args.CellFile.Replace("results.cell",
                                                                      String.Format("results_{0}.cell",
                                                                                    cellN - simN)));
          using (ts)
          {
            if (ts.LayerCount != layerCount)
            { throw new ApplicationException("Unexpected count of time layers"); }
            ts.Reset();
            int layerN = 0;
            while (ts.MoveNext())
            {
              var cell = ts.Current.Cell;
              var chr0 = cell.Chromosomes.ElementAt(0);
              var chr1 = cell.Chromosomes.ElementAt(1);

              // Count MTs
              int c0l = 0, c0r = 0, c1l = 0, c1r = 0;
              var c0 = chr0.BoundMTs;
              foreach (var mt in c0)
              {
                if (mt.Pole.Type == PoleType.Left) { c0l += 1; }
                else { c0r += 1; }
              }

              var c1 = chr1.BoundMTs;
              foreach (var mt in c1)
              {
                if (mt.Pole.Type == PoleType.Left) { c1l += 1; }
                else { c1r += 1; }
              }

              int offset = layerCount * 4 * cellN + layerN * 4;
              res_mts[offset + 0] = c0l;
              res_mts[offset + 1] = c0r;
              res_mts[offset + 2] = c1l;
              res_mts[offset + 3] = c1r;

              // Count angles
              if (cell.AreSpringsBroken) { throw new ApplicationException("Wrong setup, springs were broken!"); }
              Vector3 dp = new Vector3(cell.GetPole(PoleType.Left).Position.X - cell.GetPole(PoleType.Right).Position.X,
                                        cell.GetPole(PoleType.Left).Position.Y - cell.GetPole(PoleType.Right).Position.Y,
                                        cell.GetPole(PoleType.Left).Position.Z - cell.GetPole(PoleType.Right).Position.Z);
              if (dp.Length() == 0.0f) { dp = new Vector3(-1.0f, 0.0f, 0.0f); }
              else { dp = dp / dp.Length(); }
              if (Math.Abs(dp.Y) + Math.Abs(dp.Z) > 1e-9)
              { throw new ApplicationException("Poles must be located symmetrically"); }

              Vector3 dc_x = new Vector3((float)-chr0.Orientation[0, 0],
                                          (float)-chr0.Orientation[1, 0],
                                          (float)-chr0.Orientation[2, 0]);
              if (dc_x.Length() == 0.0f) { dc_x = new Vector3(-1.0f, 0.0f, 0.0f); }
              else { dc_x = dc_x / dc_x.Length(); }

              Vector3 dc_y = new Vector3((float)-chr0.Orientation[0, 1],
                                          (float)-chr0.Orientation[1, 1],
                                          (float)-chr0.Orientation[2, 1]);
              if (dc_y.Length() == 0.0f) { dc_y = new Vector3(0.0f, 1.0f, 0.0f); }
              else { dc_y = dc_y / dc_y.Length(); }

              offset = (layerCount * cellN + layerN) * 2;
              res_angles[offset + 0] = Math.Acos(Math.Min(1.0, Math.Max(-1.0, Vector3.Dot(dp, dc_x))));
              res_angles[offset + 1] = Math.Acos(Math.Min(1.0, Math.Max(-1.0, Vector3.Dot(dp, dc_y))));

              // Estimate the length of centromere
              offset = layerCount * cellN + layerN;
              res_centromere[offset] = cell.ChromosomePairs.ElementAt(0).Spring.Length;

              layerN += 1;
            }
          } // using ts
        }); // parallel for cellN

        if (!pfor.IsCompleted)
        { throw new ApplicationException("Failed to analyze simulation data concurrently"); }

        sw.Stop();
        Console.WriteLine(String.Format("{0} seconds", sw.ElapsedMilliseconds / 1000));
        simN += cellCount;
      } // while simN < cellsPerCase

      // Save results
      Console.WriteLine("Saving ...");

      // A-results
      using (var csv = File.CreateText("results_A.csv"))
      {
        csv.WriteLine("Connected MTs (pcs)");
        csv.Write(String.Format("Time (seconds){0}Averaged{0}Min{0}Max", csv_separator));
        for (int i = 0; i < cellsPerCase; i++)
        {
          csv.Write(String.Format("{0}Cell #{1}", csv_separator, i + 1));
        }
        csv.WriteLine();

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          StringBuilder str = new StringBuilder();
          int min = Int32.MaxValue, max = Int32.MinValue, avg = 0;
          for (int cellN = 0; cellN < cellsPerCase; cellN++)
          {
            int offset = layerN * 4 + layerCount * 4 * cellN;
            int val = res_mts[offset] + res_mts[offset + 1] + res_mts[offset + 2] + res_mts[offset + 3];
            str.Append(csv_separator);
            str.Append(val.ToString());
            min = Math.Min(min, val);
            max = Math.Max(max, val);
            avg += val;
          }
          csv.WriteLine(String.Format("{1}{0}{2}{0}{3}{0}{4}{5}", csv_separator,
                        (Save_Freq_Macro * layerN).ToString(CultureInfo.InvariantCulture),
                        ((double)avg / cellsPerCase).ToString(CultureInfo.InvariantCulture),
                        min, max, str.ToString()));
        }
      }

      // B-results
      using (var csv = File.CreateText("results_B.csv"))
      {
        csv.WriteLine("Kinetochore types (%)");
        csv.WriteLine(String.Format(
            "Time (seconds){0}No MTs{0}Monotelic{0}Syntelic{0}Bioriented{0}Merotelic", csv_separator
        ));

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          int noMTs = 0, monotelic = 0, syntelic = 0, bioriented = 0, merotelic = 0;
          for (int cellN = 0; cellN < cellsPerCase; cellN++)
          {
            int offset = layerN * 4 + layerCount * 4 * cellN;

            int c0l = res_mts[offset + 0];
            int c0r = res_mts[offset + 1];
            int c1l = res_mts[offset + 2];
            int c1r = res_mts[offset + 3];
            int c0m = Math.Min(c0l, c0r);
            int c0nm = Math.Max(c0l, c0r);
            int c1m = Math.Min(c1l, c1r);
            int c1nm = Math.Max(c1l, c1r);
            int c0_sum = c0m + c0nm;
            int c1_sum = c1m + c1nm;
            int sum = c0_sum + c1_sum;

            if (sum == 0)
            { noMTs += 1; }
            else if (c0m != 0 || c1m != 0)
            { merotelic += 1; }
            else if (c0_sum == 0 || c1_sum == 0)
            { monotelic += 1; }
            else if (Math.Min(c0l, c1l) != 0 || Math.Min(c0r, c1r) != 0)
            { syntelic += 1; }
            else if (c0m == 0 && c0nm != 0 && c1m == 0 && c1nm != 0)
            { bioriented += 1; }
            else
            { throw new ApplicationException("Internal error, unknown type of chromosome"); }
          }
          csv.WriteLine(String.Format("{1}{0}{2}{0}{3}{0}{4}{0}{5}{0}{6}", csv_separator,
                        (Save_Freq_Macro * layerN).ToString(CultureInfo.InvariantCulture),
                        ((double)noMTs / cellsPerCase).ToString(CultureInfo.InvariantCulture),
                        ((double)monotelic / cellsPerCase).ToString(CultureInfo.InvariantCulture),
                        ((double)syntelic / cellsPerCase).ToString(CultureInfo.InvariantCulture),
                        ((double)bioriented / cellsPerCase).ToString(CultureInfo.InvariantCulture),
                        ((double)merotelic / cellsPerCase).ToString(CultureInfo.InvariantCulture)));
        }
      }

      // C-results
      using (var csv = File.CreateText("results_C.csv"))
      {
        csv.WriteLine("Merotelic MTs (pcs)");
        csv.Write(String.Format("Time (seconds){0}Averaged{0}Min{0}Max", csv_separator));
        for (int i = 0; i < cellsPerCase; i++)
        {
          csv.Write(String.Format("{0}Cell #{1}", csv_separator, i + 1));
        }
        csv.WriteLine();

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          StringBuilder str = new StringBuilder();
          int min = Int32.MaxValue, max = Int32.MinValue, avg = 0;
          for (int cellN = 0; cellN < cellsPerCase; cellN++)
          {
            int offset = layerN * 4 + layerCount * 4 * cellN;
            int val = Math.Min(res_mts[offset + 0], res_mts[offset + 1]) + Math.Min(res_mts[offset + 2], res_mts[offset + 3]);
            str.Append(csv_separator);
            str.Append(val.ToString());
            min = Math.Min(min, val);
            max = Math.Max(max, val);
            avg += val;
          }
          csv.WriteLine(String.Format("{1}{0}{2}{0}{3}{0}{4}{5}", csv_separator,
                        (Save_Freq_Macro * layerN).ToString(CultureInfo.InvariantCulture),
                        ((double)avg / cellsPerCase).ToString(CultureInfo.InvariantCulture),
                        min, max, str.ToString()));
        }
      }

      // D-results
      using (var csv_x = File.CreateText("results_D_x.csv"))
      using (var csv_y = File.CreateText("results_D_y.csv"))
      {
        var csvs = new StreamWriter[] { csv_x, csv_y };
        for (int l = 0; l < csvs.Length; l++)
        {
          var csv = csvs[l];
          csv.WriteLine("Angle (degrees)");
          csv.Write(String.Format("Time (seconds){0}Averaged{0}Min{0}Max", csv_separator));
          for (int i = 0; i < cellsPerCase; i++)
          {
            csv.Write(String.Format("{0}Cell #{1}", csv_separator, i + 1));
          }
          csv.WriteLine();

          for (int layerN = 0; layerN < layerCount; layerN++)
          {
            StringBuilder str = new StringBuilder();
            double min = Double.MaxValue, max = Double.MinValue, avg = 0.0;
            for (int cellN = 0; cellN < cellsPerCase; cellN++)
            {
              double angle = res_angles[l + (layerN + layerCount * cellN) * 2];
              if (angle > Math.PI) angle = 2 * Math.PI - angle;
              if (angle > Math.PI / 2) angle = Math.PI - angle;
              angle *= 180.0 / Math.PI;
              str.Append(csv_separator);
              str.Append(String.Format(CultureInfo.InvariantCulture, "{0:F1}", angle));
              min = Math.Min(min, angle);
              max = Math.Max(max, angle);
              avg += angle;
            }

            csv.WriteLine(String.Format(CultureInfo.InvariantCulture,
                          "{1:F1}{0}{2:F1}{0}{3:F1}{0}{4:F1}{5}", csv_separator,
                          Save_Freq_Macro * layerN,
                          (double)avg / cellsPerCase,
                          min, max, str.ToString()));
          }
        }
      }

      // E-results
      using (var csv = File.CreateText("results_E.csv"))
      {
        csv.WriteLine("The length of centromere (micrometers)");
        csv.Write(String.Format("Time (seconds){0}Averaged{0}Min{0}Max", csv_separator));
        for (int i = 0; i < cellsPerCase; i++)
        {
          csv.Write(String.Format("{0}Cell #{1}", csv_separator, i + 1));
        }
        csv.WriteLine();

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          StringBuilder str = new StringBuilder();
          double min = Double.MaxValue, max = Double.MinValue, avg = 0.0;
          for (int cellN = 0; cellN < cellsPerCase; cellN++)
          {
            double length = res_centromere[layerN + layerCount * cellN] * 1e6;
            str.Append(csv_separator);
            str.Append(String.Format(CultureInfo.InvariantCulture, "{0:F2}", length));
            min = Math.Min(min, length);
            max = Math.Max(max, length);
            avg += length;
          }

          csv.WriteLine(String.Format(CultureInfo.InvariantCulture,
                        "{1:F2}{0}{2:F2}{0}{3:F2}{0}{4:F2}{5}", csv_separator,
                        Save_Freq_Macro * layerN,
                        avg / cellsPerCase,
                        min, max, str.ToString()));
        }
      }

      Directory.SetCurrentDirectory("..");
    }

    private static Experiment CreateConfig(Random rnd, int n)
    {
      Experiment exp = new Experiment();

      // Some default config as template
      var config = exp.Config;
      config[SimParameter.Int.N_MT_Total] = 1500;
      config[SimParameter.Int.N_Cr_Total] = 1;
      config[SimParameter.Int.N_KMT_Max] = 50;
      config[SimParameter.Int.Spring_Brake_Type] = 1;
      config[SimParameter.Int.Spring_Brake_MTs] = 1000;
      config[SimParameter.Int.Spring_Type] = 1;
      config[SimParameter.Double.Spring_Length] = 0.1;
      config[SimParameter.Double.Spring_K] = 13000;
      config[SimParameter.Double.L_Poles] = 10;
      config[SimParameter.Double.R_Cell] = 6;
      config[SimParameter.Double.Spring_Brake_Force] = 7000.0;
      config[SimParameter.Double.K_On] = 10.0;
      config[SimParameter.Double.K_Off] = 0.002;
      config[SimParameter.Double.Cr_L] = 3.0;
      config[SimParameter.Double.Cr_Kin_D] = 0.3;
      config[SimParameter.Double.Dt] = 0.1;
      config[SimParameter.Double.T_End] = 720.0;
      config[SimParameter.Double.Save_Freq_Macro] = 5.0;

      // Initialize poles, tubules and chromosomes
      exp.Initialize(rnd, false, true);
      exp.States.AddChromosomePair(0.0, 0.0, 1e-6, Math.PI / 2, 0.0, 0.0);

      // Tune it for the current case
      switch (n)
      {
        case 0:
        {
          config[SimParameter.Double.K_Off] = 0.004;
          exp.Name = String.Format(CultureInfo.InvariantCulture, "K_Off={0}", config[SimParameter.Double.K_Off]);
          return exp;
        }

        // Append here your cases

        default:
          return null;
      }
    }

    static void Main(string[] args)
    {
      try
      {
        Random rnd = new Random(Environment.TickCount);

        var sw = new Stopwatch();
        sw.Start();
        Experiment exp;
        int n = 0;
        while ((exp = CreateConfig(rnd, n)) != null)
        {
          var caption = String.Format("--- Simulating \"{0}\" case ---", exp.Name);
          Console.WriteLine();
          for (int j = 0; j < caption.Length; j++)
          { Console.Write('-'); }
          Console.WriteLine();
          Console.WriteLine(caption);
          for (int j = 0; j < caption.Length; j++)
          { Console.Write('-'); }
          Console.WriteLine();

          Simulate(exp, rnd);
          n += 1;
        }
        sw.Stop();
        
        Console.WriteLine();
        Console.WriteLine(String.Format("Finished! Total time - {0} seconds", sw.ElapsedMilliseconds / 1000));
      }
      catch (Exception ex)
      {
        Console.WriteLine();
        Console.WriteLine(ex.ToString());
      }
    }
  }
}
