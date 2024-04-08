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
    private static readonly int ANAPHASE_READY_MIN_KMTS = 5;
    private static readonly double ANAPHASE_READY_MAX_MEROTELIC_RATIO = 0.5;

    private static bool IsAnaphaseReady(int k0l, int k0r, int k1l, int k1r)
    {
      // You don't need anaphase if anaphase has already started
      if (k0l < 0)
      { return true; }

      // Skip cases with too little KMTs
      if (k0l + k0r < ANAPHASE_READY_MIN_KMTS || k1l + k1r < ANAPHASE_READY_MIN_KMTS)
      { return false; }

      // Skip cases when there are too many merotelic KMTs
      if ((double)Math.Min(k0l, k0r) / Math.Max(k0l, k0r) > ANAPHASE_READY_MAX_MEROTELIC_RATIO ||
          (double)Math.Min(k1l, k1r) / Math.Max(k1l, k1r) > ANAPHASE_READY_MAX_MEROTELIC_RATIO)
      { return false; }

      // Finally, skip cases when chromatids fail to become bi-oriented
      if (!((k0l < k0r) ^ (k1l < k1r)))
      { return false; }

      return true;
    }

    private static void ClassifyKinetochorePair(int k0l, int k0r, int k1l, int k1r,
                                                ref int noMTs, ref int monotelic, ref int syntelic,
                                                ref int merotelic, ref int amphitelic)
    {
      // Skip cases when anaphase has been started
      if (k0l < 0) { return; }

      // Classify kinetochore pairs y their types
      int k0m = Math.Min(k0l, k0r);
      int k0a = Math.Max(k0l, k0r);
      int k1m = Math.Min(k1l, k1r);
      int k1a = Math.Max(k1l, k1r);
      int k0_sum = k0m + k0a;
      int k1_sum = k1m + k1a;
      int sum = k0_sum + k1_sum;

      if (sum == 0)
      { noMTs += 1; }
      else if (k0m != 0 || k1m != 0)
      { merotelic += 1; }
      else if (k0_sum == 0 || k1_sum == 0)
      { monotelic += 1; }
      else if (Math.Min(k0l, k1l) != 0 || Math.Min(k0r, k1r) != 0)
      { syntelic += 1; }
      else if (k0m == 0 && k0a != 0 && k1m == 0 && k1a != 0)
      { amphitelic += 1; }
      else
      { throw new ApplicationException("Internal error, unknown type of kinetochore pair"); }
    }

    private DataTable CountTotal(AnalyzedCell[] acells, int nPairs)
    {
      var table = ConstructTable<int>(_tableFormat.Columns, acells[0].TimeTable,
                                      (uint nCell, uint nPair, uint nKin) =>
                                      {
                                        var slice = GetSlice(acells, nCell, nPair);
                                        var column = new int[slice.Length / 4];
                                        for (int i = 0; i < column.Length; i++)
                                        { column[i] = Math.Abs(slice[i * 4 + nKin * 2 + 0]) + slice[i * 4 + nKin * 2 + 1]; }
                                        return column;
                                      },
                                      (value) => value,
                                      (tbl, name, values) => tbl.AddColumn(name, values),
                                      acells.Length, nPairs);
      table.Legend = "The total number of kinetochore-microtubule attachments (KMTs)";
      return table;
    }

    private DataTable CountMerotelic(AnalyzedCell[] acells, int nPairs)
    {
      var table = ConstructTable<int>(_tableFormat.Columns, acells[0].TimeTable,
                                      (uint nCell, uint nPair, uint nKin) =>
                                      {
                                        var slice = GetSlice(acells, nCell, nPair);
                                        var column = new int[slice.Length / 4];
                                        for (int i = 0; i < column.Length; i++)
                                        {
                                          column[i] = Math.Min(Math.Abs(slice[i * 4 + nKin * 2 + 0]),
                                                                slice[i * 4 + nKin * 2 + 1]);
                                        }
                                        return column;
                                      },
                                      (value) => value,
                                      (tbl, name, values) => tbl.AddColumn(name, values),
                                      acells.Length, nPairs);
      table.Legend = "The number of merotelic kinetochore-microtubule attachments (KMTs)";
      return table;
    }

    private static DataTable ClassifyKinetochores(double[] timeCol, int[][] slices)
    {
      var noMTsCol = new double[timeCol.Length];
      var monotelicCol = new double[timeCol.Length];
      var syntelicCol = new double[timeCol.Length];
      var merotelicCol = new double[timeCol.Length];
      var amphitelicCol = new double[timeCol.Length];
      for (int nLayer = 0; nLayer < timeCol.Length; nLayer++)
      {
        int noMTs = 0, monotelic = 0, syntelic = 0, merotelic = 0, amphitelic = 0;
        for (int nSlice = 0; nSlice < slices.Length; nSlice++)
        {
          var slice = slices[nSlice];
          ClassifyKinetochorePair(slice[nLayer * 4 + 0], slice[nLayer * 4 + 1],
                                  slice[nLayer * 4 + 2], slice[nLayer * 4 + 3],
                                  ref noMTs, ref monotelic, ref syntelic, ref merotelic, ref amphitelic);
        }
        int total = noMTs + monotelic + syntelic + merotelic + amphitelic;

        noMTsCol[nLayer]      = total == 0 ? Double.NaN : (double)noMTs / total * 100;
        monotelicCol[nLayer]  = total == 0 ? Double.NaN : (double)monotelic / total * 100;
        syntelicCol[nLayer]   = total == 0 ? Double.NaN : (double)syntelic / total * 100;
        merotelicCol[nLayer]  = total == 0 ? Double.NaN : (double)merotelic / total * 100;
        amphitelicCol[nLayer] = total == 0 ? Double.NaN : (double)amphitelic / total * 100;
      }

      var table = new DataTable("Time (seconds)", timeCol);
      table.AddColumn("Without KMTs", noMTsCol);
      table.AddColumn("Monotelic", monotelicCol);
      table.AddColumn("Syntelic", syntelicCol);
      table.AddColumn("Merotelic", merotelicCol);
      table.AddColumn("Amphitelic", amphitelicCol);
      table.Legend = "Classification of kinetochores by their type in accordance with (Hauf 2004) in percents";

      return table;
    }

    private DataTable EstimateAnaphaseReadiness(AnalyzedCell[] acells, int nPairs)
    {
      var table = ConstructTable<bool>(_tableFormat.Columns, acells[0].TimeTable,
                                       (uint nCell, uint nPair) =>
                                       {
                                         var slice = GetSlice(acells, nCell, nPair);
                                         var column = new bool[slice.Length / 4];
                                         for (int i = 0; i < column.Length; i++)
                                         {
                                           column[i] = IsAnaphaseReady(slice[i * 4 + 0], slice[i * 4 + 1],
                                                                       slice[i * 4 + 2], slice[i * 4 + 3]);
                                         }
                                         return column;
                                       },
                                       (value) => value ? 1.0 : 0.0,
                                       (tbl, name, values) => tbl.AddColumn(name, values),
                                       acells.Length, nPairs);
      table.Legend = "Estimation for cells that are ready for anaphase in accordance with (Cimini 2004)'s hypothesis";
      return table;
    }
  }
}
