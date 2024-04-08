using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using MiCoSi;

namespace Experiments
{
  internal partial class KmtAnalyzer : BasicAnalyzer
  {
    /// <summary>
    /// Creates chart for the 20 random cells that are presented in the Figure 1B (start of anaphase A)
    /// </summary>
    private static Tuple<double[], double[]> Meraldi2004_Fig1B()
    {
      var times = new double[] { 0.0, 18.06, 21.11, 24.15, 27.14, 30.03, 42.06, 45.10, 60.17, 74.98, 80.97 };
      var cells = new int[] { 0, 1, 5, 3, 3, 2, 1, 2, 1, 1, 1 };
      var nCells = cells.Sum();
      var nRecords = times.Length;
      if (nRecords != cells.Length || nRecords != 11 || nCells != 20)
      { throw new ApplicationException("wrong experimental data for Fig. 1B"); }

      var resTimes  = new double[nRecords];
      var resValues = new double[nRecords];
      int accSum = 0;
      for (int i = 0; i < nRecords; i++)
      {
        accSum += cells[i];
        times[i]  = times[i] * 60.0;
        resValues[i] = (double)accSum / nCells * 100.0;
      }

      return new Tuple<double[], double[]>(times, resValues);
    }

    /// <summary>
    /// Returns averaged culmulative frequencies of cells that enter anaphase A (Fig. 1C)
    /// These values correspond to the 'average' line
    /// </summary>
    private static Tuple<double[], double[]> Meraldi2004_Fig1C()
    {
      var times  = new double[] { 0.0, 6.0, 12.0, 18.0, 24.0, 30.0, 36.0, 42.0, 48.0, 54.0, 60.0 };
      var values = new double[] { 0.0, 0.0,  0.0, 13.7, 53.5, 76.2, 83.3, 88.6, 92.5, 95.8, 95.8 };

      for (int i = 0; i < times.Length; i++) { times[i] *= 60.0; }
      return new Tuple<double[], double[]>(times, values);
    }

    /// <summary>
    /// The same as 'Meraldi2004_Fig1C()', but returns values from Fig. 1D for anaphase B
    /// </summary>
    private static Tuple<double[], double[]> Meraldi2004_Fig1D()
    {
      var times  = new double[] { 0.0, 6.0, 12.0, 18.0, 24.0, 30.0, 36.0, 42.0, 48.0, 54.0, 60.0 };
      var values = new double[] { 0.0, 0.0,  0.0,  2.5, 31.6, 67.9, 80.0, 86.6, 91.4, 93.2, 95.8 };

      for (int i = 0; i < times.Length; i++) { times[i] *= 60.0; }
      return new Tuple<double[], double[]>(times, values);
    }

    /// <summary>
    /// Counts cumulative freuency for cells that are ready for anaphase (according to 'IsAnaphaseReady()')
    /// </summary>
    private static double[] CalculateAnaphaseReadyFrequency(int[][] kmtSlices)
    {
      int nLayers = kmtSlices[0].Length / 4;
      var res = new double[nLayers];
      for (int layer = 0; layer < nLayers; layer++)
      {
        int ready = 0, total = kmtSlices.Length;
        for (int slice = 0; slice < kmtSlices.Length; slice++)
        {
          if (IsAnaphaseReady(kmtSlices[slice][layer * 4 + 0],
                              kmtSlices[slice][layer * 4 + 1],
                              kmtSlices[slice][layer * 4 + 2],
                              kmtSlices[slice][layer * 4 + 3]))
          { ready += 1; }
        }
        res[layer] = (double)ready / Math.Max(1, total) * 100.0;
      }
      return res;
    }

    /// <summary>
    /// Compares our 'AnaphaseReady' predictions with experimentally observed ones (HeLa cells)
    /// </summary>
    private static Tuple<DataTable, String>[] Meraldi2004(double[] times, int[][] kmtSlices)
    {
      var res = new List<Tuple<DataTable, String>>();
      var legend = "Timings from NEB to anaphase: https://doi.org/10.1016/j.devcel.2004.06.006";
      var values = CalculateAnaphaseReadyFrequency(kmtSlices);

      // Fig. 1B
      {
        var expLabel = "20 in vitro HeLa: anaphase A (percents)";
        var simLabel = "In silico cells (percents)";
        var fig1B = Meraldi2004_Fig1B();
        var expToSim = SelectValues(fig1B.Item1, fig1B.Item2, times);

        var detailed = new DataTable("Time (sec)", times);
        detailed.AddColumn(expLabel, expToSim);
        detailed.AddColumn(simLabel, values);
        detailed.Legend = legend;
        // res.Add(new Tuple<DataTable, String>(detailed, "Fig1B"));
      }

      // Fig. 1CD
      {
        var expLabel1C = "In vitro HeLa cells: anaphase A (percents)";
        var expLabel1D = "In vitro HeLa cells: anaphase B (percents)";
        var simLabel  = "In silico cells (percents)";
        var fig1C = Meraldi2004_Fig1C();
        var fig1D = Meraldi2004_Fig1D();

        var fig1CToSim = SelectValues(fig1C.Item1, fig1C.Item2, times);
        var fig1DToSim = SelectValues(fig1D.Item1, fig1D.Item2, times);

        var detailed = new DataTable("Time (sec)", times);
        detailed.AddColumn(expLabel1C, fig1CToSim);
        detailed.AddColumn(expLabel1D, fig1DToSim);
        detailed.AddColumn(simLabel, values);
        detailed.Legend = legend;
        res.Add(new Tuple<DataTable, String>(detailed, "Fig1CD"));
      }
      return res.ToArray();
    }
  }
}
