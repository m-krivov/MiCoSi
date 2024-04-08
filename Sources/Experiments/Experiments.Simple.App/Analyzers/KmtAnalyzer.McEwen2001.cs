using System;
using System.Collections.Generic;

namespace Experiments
{
  internal partial class KmtAnalyzer : BasicAnalyzer
  {
    private static Dictionary<int, int> McEwen2001_Fig3A()
    {
      var res = new Dictionary<int, int>();
      
      // as %
      res[13] = 2;
      res[14] = 3;
      res[15] = 14;
      res[16] = 20;
      res[17] = 20;
      res[18] = 22;
      res[19] = 11;
      res[20] = 4;
      res[21] = 3;
      res[22] = 2;

      int sum = 0;
      foreach (var rec in res)
      { sum += rec.Value; }
      if (sum != 101)   // sounds queer, but corresponds to the original chart
      { throw new ApplicationException("Wrong data from McEwen 2001, Fig. 3A"); }

      return res;
    }

    private Tuple<DataTable, String>[] McEwen2001(double[] times, int[][] kmtSlices, int N_KMT_Max)
    {
      var inVitro = McEwen2001_Fig3A();
      int nLayers = kmtSlices[0].Length / 4;
      var columns = new List<Tuple<Dictionary<int, int>, String>>();

      // Create a column for labels (the number of KMTs)
      int nLabels = N_KMT_Max;
      foreach (var rec in inVitro)
      { nLabels = Math.Max(nLabels, rec.Key); }

      var labelRows = new String[nLabels + 1];
      for (int i = 0; i < labelRows.Length; i++)
      { labelRows[i] = i.ToString(); }
      var labels = new Tuple<String[], String>(labelRows, "Number of KMT attachments per kinetochore");

      // Reference values
      columns.Add(new Tuple<Dictionary<int, int>, String>(
        inVitro,
        "In vitro HeLa cells (percentage of kinetochores)")
      );

      // Our simulations
      var timePoints = new int[] { 10, 20, 30, 40, 60, 90, 120 };
      int layer = 0;
      for (int nPoint = 0; nPoint < timePoints.Length; nPoint++)
      {
        while (layer < nLayers && times[layer] < timePoints[nPoint] * 60.0)
        { layer += 1; }
        if (layer >= nLayers) { break; }

        var values = new Dictionary<int, int>();
        for (int slice = 0; slice < kmtSlices.Length; slice++)
        {
          var arr = kmtSlices[slice];
          int left  = Math.Abs(arr[4 * layer + 0]) + arr[4 * layer + 1];
          int right =          arr[4 * layer + 2]  + arr[4 * layer + 3];
          
          if (values.ContainsKey(left))
          { values[left] = values[left] + 1; }
          else { values[left] = 1; }

          if (values.ContainsKey(right))
          { values[right] = values[right] + 1; }
          else { values[right] = 1; }
        }

        columns.Add(new Tuple<Dictionary<int, int>, String>(
          values,
          String.Format("In silico cells at {0}-th min after NEB (percentage of kinetochores)", timePoints[nPoint])
        ));
      }

      var res = ConstructHistogram(labels, columns.ToArray(), 1, true);
      res.Legend  = "How metaphase cells are distributed by the number of KMT attachments";
      res.Legend += ": https://doi.org/10.1091/mbc.12.9.2776";
      return new Tuple<DataTable, String>[] { new Tuple<DataTable, String>(res, "Fig3A") };
    }
  }
}
