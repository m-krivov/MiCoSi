using System;
using System.Collections.Generic;
using System.Linq;

namespace Experiments
{
  internal partial class KmtAnalyzer : BasicAnalyzer
  {
    private static Tuple<double[], double[]> Rieder1994_Fig3A()
    {
      var times = new double[] {  0.0,  5.0, 10.0, 15.0, 20.0, 25.0,
                                 30.0, 35.0, 40.0, 45.0, 50.0, 55.0,
                                 60.0, 65.0, 70.0, 75.0, 80.0, 85.0,
                                 90.0, 95.0 };
      var values = new int[] {  0,  0,  0,  0,  1, 12,
                               13, 19, 16, 13, 13, 10,
                                7,  3,  6,  0,  6,  4,
                                1,  2 };
      var nCells = values.Sum();
      if (times.Length != values.Length || nCells != 126)
      { throw new ApplicationException("wrong data from Fig. 3A"); }

      var frequency = new double[values.Length];
      int accSum = 0;
      for (int i = 0; i < times.Length; i++)
      {
        times[i] *= 60.0;
        accSum += values[i];
        frequency[i] = (double)accSum / nCells * 100.0;
      }

      return new Tuple<double[], double[]>(times, frequency);
    }

    private static Tuple<double[], double[]> Rieder1994_Fig3B_Points()
    {
      var points = new List<Tuple<double, double>>();

      // The lower triangle: 0 - 9
      points.Add(new Tuple<double, double>(8.5, 25.2));
      points.Add(new Tuple<double, double>(8.7, 31.3));
      points.Add(new Tuple<double, double>(9.7, 27.9));
      points.Add(new Tuple<double, double>(9.8, 26.2));
      points.Add(new Tuple<double, double>(10.6, 25.2));
      points.Add(new Tuple<double, double>(11.8, 25.2));
      points.Add(new Tuple<double, double>(11.8, 32.7));
      points.Add(new Tuple<double, double>(12.7, 23.0));
      points.Add(new Tuple<double, double>(12.7, 28.6));
      points.Add(new Tuple<double, double>(13.8, 28.4));

      // The lower triangle: 10 - 19
      points.Add(new Tuple<double, double>(13.8, 34.9));
      points.Add(new Tuple<double, double>(15.0, 32.7));
      points.Add(new Tuple<double, double>(15.8, 25.0));
      points.Add(new Tuple<double, double>(15.8, 33.0));
      points.Add(new Tuple<double, double>(17.1, 34.7));
      points.Add(new Tuple<double, double>(17.3, 33.2));
      points.Add(new Tuple<double, double>(17.9, 33.5));
      points.Add(new Tuple<double, double>(17.9, 29.3));
      points.Add(new Tuple<double, double>(18.1, 36.1));
      points.Add(new Tuple<double, double>(19.7, 35.6));

      // The lower triangle: 20 - 29
      points.Add(new Tuple<double, double>(19.9, 33.9));
      points.Add(new Tuple<double, double>(20.2, 37.3));
      points.Add(new Tuple<double, double>(21.2, 40.7));
      points.Add(new Tuple<double, double>(21.9, 34.2));
      points.Add(new Tuple<double, double>(21.9, 43.2));
      points.Add(new Tuple<double, double>(22.0, 35.4));
      points.Add(new Tuple<double, double>(22.5, 38.1));
      points.Add(new Tuple<double, double>(22.7, 43.2));
      points.Add(new Tuple<double, double>(22.8, 39.0));
      points.Add(new Tuple<double, double>(23.8, 35.2));

      // The lower triangle: 30 - 39
      points.Add(new Tuple<double, double>(23.8, 38.1));
      points.Add(new Tuple<double, double>(23.9, 42.9));
      points.Add(new Tuple<double, double>(24.8, 38.8));
      points.Add(new Tuple<double, double>(24.8, 44.9));
      points.Add(new Tuple<double, double>(25.9, 44.9));
      points.Add(new Tuple<double, double>(26.8, 32.0));
      points.Add(new Tuple<double, double>(26.8, 42.9));
      points.Add(new Tuple<double, double>(27.1, 48.0));
      points.Add(new Tuple<double, double>(27.9, 39.0));
      points.Add(new Tuple<double, double>(27.9, 40.2));

      // The lower triangle: 40 - 49
      points.Add(new Tuple<double, double>(29.9, 51.8));
      points.Add(new Tuple<double, double>(31.4, 51.9));
      points.Add(new Tuple<double, double>(34.9, 52.9));
      points.Add(new Tuple<double, double>(34.9, 54.8));
      points.Add(new Tuple<double, double>(35.9, 52.9));
      points.Add(new Tuple<double, double>(36.8, 55.5));
      points.Add(new Tuple<double, double>(40.9, 60.9));
      points.Add(new Tuple<double, double>(41.1, 57.5));
      points.Add(new Tuple<double, double>(41.8, 56.3));
      points.Add(new Tuple<double, double>(41.8, 63.5));

      // The lower triangle: 50 - 58
      points.Add(new Tuple<double, double>(42.8, 61.6));
      points.Add(new Tuple<double, double>(47.8, 64.0));
      points.Add(new Tuple<double, double>(47.8, 67.6));
      points.Add(new Tuple<double, double>(48.9, 68.9));
      points.Add(new Tuple<double, double>(52.6, 69.8));
      points.Add(new Tuple<double, double>(57.1, 80.0));
      points.Add(new Tuple<double, double>(59.7, 79.8));
      points.Add(new Tuple<double, double>(65.8, 86.8));
      points.Add(new Tuple<double, double>(82.6, 89.5));

      // The upper triangle: 0 - 9
      points.Add(new Tuple<double, double>(4.8, 34.4));
      points.Add(new Tuple<double, double>(8.0, 35.2));
      points.Add(new Tuple<double, double>(13.1, 36.9));
      points.Add(new Tuple<double, double>(13.8, 36.9));
      points.Add(new Tuple<double, double>(8.9, 37.1));
      points.Add(new Tuple<double, double>(10.9, 37.8));
      points.Add(new Tuple<double, double>(9.8, 39.3));
      points.Add(new Tuple<double, double>(16.0, 41.0));
      points.Add(new Tuple<double, double>(13.9, 41.0));
      points.Add(new Tuple<double, double>(10.9, 41.0));

      // The upper triangle: 10 - 19
      points.Add(new Tuple<double, double>(12.3, 41.2));
      points.Add(new Tuple<double, double>(17.3, 42.2));
      points.Add(new Tuple<double, double>(15.0, 42.4));
      points.Add(new Tuple<double, double>(20.3, 43.9));
      points.Add(new Tuple<double, double>(13.9, 45.1));
      points.Add(new Tuple<double, double>(19.0, 45.8));
      points.Add(new Tuple<double, double>(20.8, 46.1));
      points.Add(new Tuple<double, double>(8.0, 46.8));
      points.Add(new Tuple<double, double>(9.8, 46.8));
      points.Add(new Tuple<double, double>(9.8, 48.0));

      // The upper triangle: 20 - 29
      points.Add(new Tuple<double, double>(17.1, 48.5));
      points.Add(new Tuple<double, double>(25.5, 49.0));
      points.Add(new Tuple<double, double>(23.8, 49.2));
      points.Add(new Tuple<double, double>(19.6, 49.7));
      points.Add(new Tuple<double, double>(8.0, 49.9));
      points.Add(new Tuple<double, double>(14.1, 50.9));
      points.Add(new Tuple<double, double>(22.8, 50.9));
      points.Add(new Tuple<double, double>(23.8, 51.9));
      points.Add(new Tuple<double, double>(14.1, 52.6));
      points.Add(new Tuple<double, double>(25.8, 52.6));

      // The upper triangle: 30 - 39
      points.Add(new Tuple<double, double>(29.9, 53.6));
      points.Add(new Tuple<double, double>(25.1, 51.4));
      points.Add(new Tuple<double, double>(22.9, 54.8));
      points.Add(new Tuple<double, double>(16.0, 56.0));
      points.Add(new Tuple<double, double>(13.2, 57.7));
      points.Add(new Tuple<double, double>(25.9, 60.9));
      points.Add(new Tuple<double, double>(38.1, 63.0));
      points.Add(new Tuple<double, double>(37.7, 65.9));
      points.Add(new Tuple<double, double>(44.6, 70.6));
      points.Add(new Tuple<double, double>(34.9, 71.8));

      // The upper triangle: 40 - 49
      points.Add(new Tuple<double, double>(37.8, 72.3));
      points.Add(new Tuple<double, double>(49.2, 73.0));
      points.Add(new Tuple<double, double>(47.6, 74.2));
      points.Add(new Tuple<double, double>(52.6, 81.0));
      points.Add(new Tuple<double, double>(56.6, 81.7));
      points.Add(new Tuple<double, double>(48.9, 82.7));
      points.Add(new Tuple<double, double>(36.8, 84.1));
      points.Add(new Tuple<double, double>(54.6, 84.6));
      points.Add(new Tuple<double, double>(55.9, 86.1));
      points.Add(new Tuple<double, double>(41.1, 88.7));

      // The upper triangle: 50 - 51
      points.Add(new Tuple<double, double>(63.6,  95.5));
      points.Add(new Tuple<double, double>(168.6, 196.9));

      // Convert point into two arrays of seconds
      var lastMono = new double[points.Count];
      var anaphase = new double[points.Count];
      for (int i = 0; i < points.Count; i++)
      {
        lastMono[i] = points[i].Item1 * 60.0;
        anaphase[i] = points[i].Item2 * 60.0;
      }
      return new Tuple<double[], double[]>(lastMono, anaphase);
    }

    private static Tuple<double[], double[]> LastMonoorientedToAnaphase(double[] times, int[][] kmtSlices)
    {
      // Determine points 
      var points = new List<Tuple<double, double>>();
      foreach (var slice in kmtSlices)
      {
        // The last moment when cell was not ready for anaphase (if any exists)
        double tAnaphase = Double.NaN;
        {
          int moment = times.Length - 1;
          while (moment >= 0 &&
                 IsAnaphaseReady(slice[4 * moment + 0], slice[4 * moment + 1],
                                 slice[4 * moment + 2], slice[4 * moment + 3]))
          { moment -= 1; }

          if (moment >= 0 && moment < times.Length - 1)
          { tAnaphase = times[moment]; }
        }

        // The last moment when kinetochore pair was mono-oriented (if any exists)
        double tLastMonooriented = Double.NaN;
        {
          int moment = times.Length - 1;
          while (moment >= 0)
          {
            int noMTs = 0, monotelic = 0, syntelic = 0, merotelic = 0, amphitelic = 0;
            ClassifyKinetochorePair(slice[4 * moment + 0], slice[4 * moment + 1],
                                    slice[4 * moment + 2], slice[4 * moment + 3],
                                    ref noMTs, ref monotelic, ref syntelic,
                                    ref merotelic, ref amphitelic);
            if (merotelic + amphitelic == 0)
            { break; }
            moment -= 1;
          }

          if (moment >= 0 && moment < times.Length - 1)
          { tLastMonooriented = times[moment]; }
        }

        points.Add(new Tuple<double, double>(tLastMonooriented, tAnaphase));
      }

      // Convert them to two arrays
      var lastMonooriented = new double[points.Count];
      var anaphase = new double[points.Count];
      for (int i = 0; i < points.Count; i++)
      {
        lastMonooriented[i] = points[i].Item1;
        anaphase[i] = points[i].Item2;
      }
      return new Tuple<double[], double[]>(lastMonooriented, anaphase);
    }

    /// <summary>
    /// Compares our 'AnaphaseReady' predictions with experimentally observed ones (PtK1 cells)
    /// </summary>
    private static Tuple<DataTable, String>[] Rieder1994(double[] times, int[][] kmtSlices)
    {
      var res = new List<Tuple<DataTable, String>>();

      // Fig. 3A
      {
        var table = new DataTable("Time (sec)", times);
        var invitro = Rieder1994_Fig3A();
        table.AddColumn("In vitro PtK1 cells (percents)", SelectValues(invitro.Item1, invitro.Item2, times));
        table.AddColumn("In silico cells (percents)", CalculateAnaphaseReadyFrequency(kmtSlices));
        table.Legend = "Timings from NEB to anaphase: https://doi.org/10.1083/jcb.127.5.1301";
        res.Add(new Tuple<DataTable, String>(table, "Fig3A"));
      }

      // Fig. 3B
      {
        var inVitroPoints = Rieder1994_Fig3B_Points();
        var inSilicoPoints = LastMonoorientedToAnaphase(times, kmtSlices);

        var timeColumn = new List<double>();
        timeColumn.AddRange(inVitroPoints.Item1);
        timeColumn.AddRange(inSilicoPoints.Item1);

        var inVitroColumn = new List<double>();
        inVitroColumn.AddRange(inVitroPoints.Item2);
        for (int i = 0; i < inSilicoPoints.Item2.Length; i++)
        { inVitroColumn.Add(Double.NaN); }

        var inSilicoColumn = new List<double>();
        for (int i = 0; i < inVitroPoints.Item2.Length; i++)
        { inSilicoColumn.Add(Double.NaN); }
        inSilicoColumn.AddRange(inSilicoPoints.Item2);

        var pointTable = new DataTable("NEB to last mono-oriented (sec)", timeColumn.ToArray());
        pointTable.AddColumn("In vitro PtK1 cells: NEB to anaphase (sec)", inVitroColumn.ToArray());
        pointTable.AddColumn("In silico cells: NEB to anaphase (sec)", inSilicoColumn.ToArray());
        pointTable.Legend  = "The dependency between the anaphase time and";
        pointTable.Legend += " the moment when the last mono-oriented chromosome becomes bi-oriented";
        pointTable.Legend += ": https://doi.org/10.1083/jcb.127.5.1301";
        res.Add(new Tuple<DataTable, string>(pointTable, "Fig3B"));
      }

      return res.ToArray();
    }
  }
}
