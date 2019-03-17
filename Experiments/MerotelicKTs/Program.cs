using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Globalization;
using System.Diagnostics;
using Mitosis;

namespace MtCounter
{
  class Program
  {
    private static bool allConfigs = false;         //default only or all nine configs
    private static int simCount = 5;                //the number of simulations
    private static bool hasLength = false;          //enable initial length
    private static bool hasDiff   = true;           //enable diffusion
    private static bool movePoles = true;           //move poles from top to sides

    private static void Simulate(String testID, SimParams config, Random rnd)
    {
      double r = config[SimParameter.Double.R_Cell];
      double l_poles = config[SimParameter.Double.L_Poles];

      //Preparing directory.
      if (Directory.Exists(testID))
        Directory.Delete(testID, true);
      Directory.CreateDirectory(testID);
      Directory.SetCurrentDirectory(testID);

      //Parsing some used values.
      int MtCount = config[SimParameter.Int.N_MT_Total];
      double dt = config[SimParameter.Double.Dt];
      double saveFreq = config[SimParameter.Double.Save_Freq_Macro];
      double tend = config[SimParameter.Double.T_End];
      int layerCount = (int)(tend / saveFreq) + 1;

      //Prepared, starting.
      int[] res_mts = new int[layerCount * 4 * simCount];
      double[] res_angles = new double[layerCount * 2 * simCount];
      Stopwatch sw = new Stopwatch();
      for (int simN = 0; simN < simCount; simN++)
      {
        Console.WriteLine("Test #" + (simN + 1).ToString());

        //Simulating.
        Console.Write("   Simulating ... ");
        var parameters = new LaunchParameters();
        sw.Reset();
        sw.Start();

        //Applying patch.
        parameters.Config = config;
        config[SimParameter.Double.D_Trans] = hasDiff ? SimParams.GetDefaultValue(SimParameter.Double.D_Trans, false) : 0.0;
        config[SimParameter.Double.D_Rot] = hasDiff ? SimParams.GetDefaultValue(SimParameter.Double.D_Rot, false) : 0.0;

        //Initial state.
        parameters.InitialStates = new InitialStates();
        parameters.InitialStates.AddChromosomePair(0.0, 0.0, 1e-6, Math.PI / 2, 0.0, 0.0);
        for (int i = 0; i < MtCount * 2; i++)
        {
          double alpha = (rnd.NextDouble()) * Math.PI * 2;
          double dx = rnd.NextDouble() * (i < MtCount ? 1 : -1);
          double dy = Math.Sqrt(1.0 - dx * dx) * Math.Cos(alpha);
          double dz = Math.Sqrt(1.0 - dx * dx) * Math.Sin(alpha);

          if (hasLength)
          {
            double a = i < MtCount ? -l_poles / 2 : l_poles / 2;
            double t = 0.0;
            if (movePoles)
              t = -dy * a + Math.Sqrt(a * a * (dy * dy - 1) + r * r) * (i < MtCount ? -1.0 : 1.0);
            else
              t = -dx * a + Math.Sqrt(a * a * (dx * dx - 1) + r * r);
            double max_l = Math.Sqrt((dx * t) * (dx * t) + (dy * t) * (dy * t) + (dz * t) * (dz * t));
            parameters.InitialStates.AddMT(i < MtCount ? PoleType.Left : PoleType.Right,
                                           dx, dy, dz,
                                           rnd.NextDouble() * max_l * 1e-6,
                                           (rnd.Next() % 2 == 0) ? MTState.Polymerization : MTState.Depolymerization);
          }
          else
          {
            parameters.InitialStates.AddMT(i < MtCount ? PoleType.Left : PoleType.Right,
                                           dx, dy, dz,
                                           0.0,
                                           MTState.Polymerization);
          }
        }

        //Poles.
        if (movePoles)
        {
          parameters.PoleCoords = new PoleCoordinates();

          double t = 180.0;
          double pole_r = l_poles / 2 * 1e-6;
          for (int i = 0; i < t; i++)
          {
            double angle = i / (t - 1) * Math.PI / 2;
            parameters.PoleCoords.AddRecord((double)i,
                                            -pole_r * Math.Sin(angle), pole_r * Math.Cos(angle), 0.0,
                                            pole_r * Math.Sin(angle), pole_r * Math.Cos(angle), 0.0);
          }
        }

        //Launching.
        parameters.Args.Mode = LaunchMode.New;
        parameters.Args.Solver = new SimulatorConfig(SimulatorType.EXPERIMENTAL);
        Launcher ml = new Launcher(".", "../Mitosis.exe", parameters);
        var res = ml.StartAndWait();
        if (res.ExitCode != 0 || !String.IsNullOrEmpty(res.Error))
          throw new ApplicationException(String.Format("Failed to simulate, error #{0}: {1}, {2}",
                                                       res.ExitCode, res.Error, res.Output));
        ml.Dispose();
        sw.Stop();
        Console.WriteLine(String.Format("{0} seconds", (int)sw.ElapsedMilliseconds / 1000));

        //Counting.
        Console.Write("   Counting ... ");
        sw.Reset();
        sw.Start();
        var ts = TimeStream.Open(parameters.Args.CellFile);
        if (ts.LayerCount != layerCount)
          throw new ApplicationException("Unexpected count of time layers");
        ts.Reset();
        int layerN = 0;
        while (ts.MoveNext())
        {
          var cell = ts.Current.Cell;
          var chr0 = cell.Chromosomes.ElementAt(0);
          var chr1 = cell.Chromosomes.ElementAt(1);

          //Counting MTs.
          int c0l = 0, c0r = 0, c1l = 0, c1r = 0;
          var c0 = chr1.BoundMTs;
          foreach (var mt in c0)
            if (mt.Pole.Type == PoleType.Left) c0l += 1;
            else c0r += 1;

          var c1 = chr1.BoundMTs;
          foreach (var mt in c1)
            if (mt.Pole.Type == PoleType.Left) c1l += 1;
            else c1r += 1;

          int offset = layerCount * 4 * simN + layerN * 4;
          res_mts[offset + 0] = c0l;
          res_mts[offset + 1] = c0r;
          res_mts[offset + 2] = c1l;
          res_mts[offset + 3] = c1r;

          //Counting angles.
          if (cell.AreSpringsBroken) throw new ApplicationException("Wrong setup, springs were broken!");
          Vec3 dp = new Vec3(cell.GetPole(PoleType.Left).Position.X - cell.GetPole(PoleType.Right).Position.X,
                             cell.GetPole(PoleType.Left).Position.Y - cell.GetPole(PoleType.Right).Position.Y,
                             cell.GetPole(PoleType.Left).Position.Z - cell.GetPole(PoleType.Right).Position.Z);
          if (dp.Length == 0.0) { dp = new Vec3(-1.0, 0.0, 0.0); }
          else { dp = dp / dp.Length; }
          if (Math.Abs(dp.y) + Math.Abs(dp.z) > 1e-9)
            throw new ApplicationException("Poles must be located symmetrically");

          Vec3 dc_x = new Vec3(-chr0.Orientation[0, 0], -chr0.Orientation[1, 0], -chr0.Orientation[2, 0]);
          if (dc_x.Length == 0.0) { dc_x = new Vec3(-1.0, 0.0, 0.0); }
          else { dc_x = dc_x / dc_x.Length; }

          Vec3 dc_y = new Vec3(-chr0.Orientation[0, 1], -chr0.Orientation[1, 1], -chr0.Orientation[2, 1]);
          if (dc_y.Length == 0.0) { dc_y = new Vec3(0.0, 1.0, 0.0); }
          else { dc_y = dc_y / dc_y.Length; }

          offset = (layerCount * simN + layerN) * 2;
          res_angles[offset + 0] = Math.Acos(Math.Min(1.0, Math.Max(-1.0, Vec3.DotProduct(dp, dc_x))));
          res_angles[offset + 1] = Math.Acos(Math.Min(1.0, Math.Max(-1.0, Vec3.DotProduct(dp, dc_y))));

          layerN += 1;
        }
        ts.Dispose();
        sw.Stop();
        Console.WriteLine(String.Format("{0} seconds", sw.ElapsedMilliseconds / 1000));
      }

      //Saving.
      Console.WriteLine("Saving ...");

      //A-results.
      {
        var csv = File.CreateText("results_A.csv");
        csv.WriteLine("Connected MTs (pcs)");
        csv.Write("Time (seconds),Averaged,Min,Max");
        for (int i = 0; i < simCount; i++) csv.Write(",Test #" + (i + 1).ToString());
        csv.WriteLine();

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          StringBuilder str = new StringBuilder();
          int min = Int32.MaxValue, max = Int32.MinValue, avg = 0;
          for (int simN = 0; simN < simCount; simN++)
          {
            int offset = layerN * 4 + layerCount * 4 * simN;
            int val = res_mts[offset] + res_mts[offset + 1] + res_mts[offset + 2] + res_mts[offset + 3];
            str.Append(",");
            str.Append(val.ToString());
            min = Math.Min(min, val);
            max = Math.Max(max, val);
            avg += val;
          }
          csv.WriteLine(String.Format("{0},{1},{2},{3}{4}",
                        (saveFreq * layerN).ToString(CultureInfo.InvariantCulture),
                        ((double)avg / simCount).ToString(CultureInfo.InvariantCulture),
                        min, max, str.ToString()));
        }
        csv.Dispose();
      }

      //B-results.
      {
        var csv = File.CreateText("results_B.csv");
        csv.WriteLine("Chromosome types (%)");
        csv.WriteLine("Time (seconds),No MTs,Monotelic,Syntelic,Bioriented,Merotelic");

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          int noMTs = 0, monotelic = 0, syntelic = 0, bioriented = 0, merotelic = 0;
          for (int simN = 0; simN < simCount; simN++)
          {
            int offset = layerN * 4 + layerCount * 4 * simN;

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
              noMTs += 1;
            else if (c0m != 0 || c1m != 0)
              merotelic += 1;
            else if (c0_sum == 0 || c1_sum == 0)
              monotelic += 1;
            else if (Math.Min(c0l, c1l) != 0 || Math.Min(c0r, c1r) != 0)
              syntelic += 1;
            else if (c0m == 0 && c0nm != 0 && c1m == 0 && c1nm != 0)
              bioriented += 1;
            else
              throw new ApplicationException("Internal error, unknown type of chromosome");
          }
          csv.WriteLine(String.Format("{0},{1},{2},{3},{4},{5}",
                        (saveFreq * layerN).ToString(CultureInfo.InvariantCulture),
                        ((double)noMTs / simCount).ToString(CultureInfo.InvariantCulture),
                        ((double)monotelic / simCount).ToString(CultureInfo.InvariantCulture),
                        ((double)syntelic / simCount).ToString(CultureInfo.InvariantCulture),
                        ((double)bioriented / simCount).ToString(CultureInfo.InvariantCulture),
                        ((double)merotelic / simCount).ToString(CultureInfo.InvariantCulture)));
        }
        csv.Dispose();
      }

      //C-results.
      {
        var csv = File.CreateText("results_C.csv");
        csv.WriteLine("Merotelic MTs (pcs)");
        csv.Write("Time (seconds),Averaged,Min,Max");
        for (int i = 0; i < simCount; i++) csv.Write(",Test #" + (i + 1).ToString());
        csv.WriteLine();

        for (int layerN = 0; layerN < layerCount; layerN++)
        {
          StringBuilder str = new StringBuilder();
          int min = Int32.MaxValue, max = Int32.MinValue, avg = 0;
          for (int simN = 0; simN < simCount; simN++)
          {
            int offset = layerN * 4 + layerCount * 4 * simN;
            int val = Math.Min(res_mts[offset + 0], res_mts[offset + 1]) + Math.Min(res_mts[offset + 2], res_mts[offset + 3]);
            str.Append(",");
            str.Append(val.ToString());
            min = Math.Min(min, val);
            max = Math.Max(max, val);
            avg += val;
          }
          csv.WriteLine(String.Format("{0},{1},{2},{3}{4}",
                        (saveFreq * layerN).ToString(CultureInfo.InvariantCulture),
                        ((double)avg / simCount).ToString(CultureInfo.InvariantCulture),
                        min, max, str.ToString()));
        }
        csv.Dispose();
      }

      //D-results.
      {
        using (var csv_x = File.CreateText("results_D_x.csv"))
        using (var csv_y = File.CreateText("results_D_y.csv"))
        {
          var csvs = new StreamWriter[] { csv_x, csv_y };
          for (int l = 0; l < csvs.Length; l++)
          {
            var csv = csvs[l];
            csv.WriteLine("Angle, degrees");
            csv.Write("Time (seconds),Averaged,Min,Max");
            for (int i = 0; i < simCount; i++) csv.Write(",Test #" + (i + 1).ToString());
            csv.WriteLine();

            for (int layerN = 0; layerN < layerCount; layerN++)
            {
              StringBuilder str = new StringBuilder();
              double min = Double.MaxValue, max = Double.MinValue, avg = 0.0;
              for (int simN = 0; simN < simCount; simN++)
              {
                double angle = res_angles[l + (layerN + layerCount * simN) * 2];
                if (angle > Math.PI) angle = 2 * Math.PI - angle;
                if (angle > Math.PI / 2) angle = Math.PI - angle;
                angle *= 180.0 / Math.PI;
                str.Append(",");
                str.Append(String.Format(CultureInfo.InvariantCulture,
                             "{0:F1}", angle));
                min = Math.Min(min, angle);
                max = Math.Max(max, angle);
                avg += angle;
              }

              csv.WriteLine(String.Format(CultureInfo.InvariantCulture,
                            "{0:F1},{1:F1},{2:F1},{3:F1}{4}",
                            saveFreq * layerN,
                            (double)avg / simCount,
                            min, max, str.ToString()));
            }
          }
        }
      }

      Directory.SetCurrentDirectory("..");
    }

    private static int ConfigCount
    {
      get { return allConfigs ? 9 : 1; }
    }

    private static Tuple<SimParams, String> CreateConfig(int n)
    {
      //Creating default config.
      var config = new SimParams();
      config[SimParameter.Int.N_MT_Total] = 1500;
      config[SimParameter.Int.N_Cr_Total] = 1;
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
      config[SimParameter.Double.T_End] = 7200.0;
      config[SimParameter.Double.Save_Freq_Macro] = 0.5;

      //Settings updates
      if (n == 0)
        return new Tuple<SimParams, String>(config, "Default");
      else if (n == 1 || n == 2)
      {
        config[SimParameter.Int.N_MT_Total] = (n % 2 == 1) ? 1000 : 2000;
        return new Tuple<SimParams, String>(config, String.Format(CultureInfo.InvariantCulture, "N_MT_Total={0}", config[SimParameter.Int.N_MT_Total]));
      }
      else if (n == 3 || n == 4)
      {
        config[SimParameter.Double.K_Off] = (n % 2 == 1) ? 0.001 : 0.004;
        return new Tuple<SimParams, String>(config, String.Format(CultureInfo.InvariantCulture, "K_Off={0}", config[SimParameter.Double.K_Off]));
      }
      else if (n == 5 || n == 6)
      {
        config[SimParameter.Double.Cr_Kin_D] = (n % 2 == 1) ? 0.2 : 0.5;
        return new Tuple<SimParams, String>(config, String.Format(CultureInfo.InvariantCulture, "Cr_Kin_D={0}", config[SimParameter.Double.Cr_Kin_D]));
      }
      else if (n == 7 || n == 8)
      {
        config[SimParameter.Double.L_Poles] = (n % 2 == 1) ? 6 : 14;
        config[SimParameter.Double.R_Cell] = (n % 2 == 1) ? 4 : 8;
        return new Tuple<SimParams, String>(config, String.Format(CultureInfo.InvariantCulture, "L_Poles={0},R_Cell={1}", config[SimParameter.Double.L_Poles], config[SimParameter.Double.R_Cell]));
      }
      else
        throw new ApplicationException("Wrong config number");
    }

    static void Main(string[] args)
    {
      try
      {
        Random rnd = new Random(Environment.TickCount);

        var sw = new Stopwatch();
        sw.Start();
        for (int i = 0; i < ConfigCount; i++)
        {
          var tuple = CreateConfig(i);
          var caption = String.Format("--- Simulating \"{0}\" case [{1}/{2}] ---", tuple.Item2, i + 1, ConfigCount);
          Console.WriteLine();
          for (int j = 0; j < caption.Length; j++)
            Console.Write('-');
          Console.WriteLine();
          Console.WriteLine(caption);
          for (int j = 0; j < caption.Length; j++)
            Console.Write('-');
          Console.WriteLine();
          Simulate(tuple.Item2, tuple.Item1, rnd);
        }
        sw.Stop();
        
        Console.WriteLine();
        Console.WriteLine(String.Format("Finished! Total time - {0} seconds", sw.ElapsedMilliseconds / 1000));
      }
      catch (Exception ex)
      {
        Console.WriteLine(ex.ToString());
      }
    }
  }
}
