using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Globalization;
using System.Diagnostics;
using MiCoSi;

namespace Experiments
{
  class Program
  {
    /// <summary>
    /// A string identifier to differ these simulations from others
    /// It will be used as a name for directory with the results
    /// Some ideas: "Article", "Diploma", "HeLa", "Kinetochores", etc
    /// </summary>
    static readonly String ID = "Sample";

    static void Configure(Experiment exp)
    {
      // Our framework may be customized for your case
      // First of all, configure the table format
      exp.TableFormat.Columns.MinValue = true;
      exp.TableFormat.Columns.MaxValue = true;
      exp.TableFormat.Columns.Average = true;
      exp.TableFormat.Columns.Median = true;
      exp.TableFormat.Columns.Dispersion = true;
      exp.TableFormat.Columns.Values = true;
      exp.TableFormat.CsvSeparator = ';';
      exp.TableFormat.Precision = 3;

      // In addition, you may update the way how to perform computations
      exp.BatchSize = 0;
      exp.Logging = Console.Out;
      exp.Seed = 12092021;

      // After that, enable analyzers which you want to get the data from
      exp.Analyzers.Centromere = true;
      exp.Analyzers.KMTs.Total = true;
      exp.Analyzers.KMTs.Merotelic = true;
      exp.Analyzers.KMTs.ByTypes = true;
      exp.Analyzers.KMTs.AnaphaseReady = true;
      exp.Analyzers.KMTs.Dudka2018 = true;
      exp.Analyzers.KMTs.McEwen2001 = true;
      exp.Analyzers.KMTs.Meraldi2004 = true;
      exp.Analyzers.KMTs.Rieder1994 = true;
      exp.Analyzers.Movement.Orientation = true;
      exp.Analyzers.Movement.Magidson2011 = true;
    }

    static Specification[] Init()
    {
      List<Specification> specs = new List<Specification>();
      Specification spec = null;

      // Place here the cell configuration that you want to analyze
      // If you need to perform an independent simulations, just create a copies of this section
      {
        spec = new Specification();
        spec.Name = "default";

        spec.MovePoles = false;
        spec.Cells = 8;
        spec.Config = new SimParams();
        spec.Config[SimParameter.Int.N_MT_Total] = 1500;
        spec.Config[SimParameter.Int.N_Cr_Total] = 1;
        spec.Config[SimParameter.Int.N_KMT_Max] = 50;
        spec.Config[SimParameter.Int.Spring_Brake_Type] = 1;
        spec.Config[SimParameter.Int.Spring_Brake_MTs] = 1000;
        spec.Config[SimParameter.Int.Spring_Type] = 1;
        spec.Config[SimParameter.Double.Spring_Length] = 0.1;
        spec.Config[SimParameter.Double.Spring_K] = 13000;
        spec.Config[SimParameter.Double.L_Poles] = 10;
        spec.Config[SimParameter.Double.R_Cell] = 6;
        spec.Config[SimParameter.Double.Spring_Brake_Force] = 7000.0;
        spec.Config[SimParameter.Double.K_On] = 10.0;
        spec.Config[SimParameter.Double.K_Off] = 0.002;
        spec.Config[SimParameter.Double.Cr_L] = 3.0;
        spec.Config[SimParameter.Double.Cr_Kin_D] = 0.3;
        spec.Config[SimParameter.Double.Dt] = 0.1;
        spec.Config[SimParameter.Double.T_End] = 7200.0;
        spec.Config[SimParameter.Double.Save_Freq_Macro] = 5.0;
        spec.Chromosomes = new List<Tuple<Vector3, Vector3>>();
        spec.Chromosomes.Add(new Tuple<Vector3, Vector3>(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 0.0f, 0.0f)));

        specs.Add(spec);
      }

      return specs.ToArray();
    }
  
    static void Main(string[] args)
    {
      Console.WriteLine(String.Format("Using MiCoSi v{0} ({1})",
                        MiCoSi.CurrentVersion.ToString(),
                        MiCoSi.CurrentVersion.PublishDate));

      var exp = new Experiment("MiCoSi.exe");
      Configure(exp);

      Specification[] specs = Init();
      exp.Perform(Path.Combine(Directory.GetCurrentDirectory(), ID), specs);
    }
  }
}
