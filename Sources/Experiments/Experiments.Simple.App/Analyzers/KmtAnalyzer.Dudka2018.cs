using System;

namespace Experiments
{
  internal partial class KmtAnalyzer : BasicAnalyzer
  {
    /// <summary>
    /// Anaphase timings for cells that are used as reference
    /// </summary>
    private static Tuple<double[], double[]> Dudka2018_Fig3A_DMSO()
    {
      double[] times    = new double[] { 0,   5,   10,  15,   20,   25    };
      double[] percents = new double[] { 0.0, 0.0, 5.3, 59.1, 96.6, 100.0 };
      if (times.Length != percents.Length || times.Length != 6)
      { throw new ApplicationException("Wrong data for Dudka 2018 (Fig. 3A)"); }

      for (int i = 0; i < times.Length; i++)
      { times[i] *= 60.0; }
      return new Tuple<double[], double[]>(times, percents);
    }

    /// <summary>
    /// Anaphase timings for cells with suppressed kinetochore binding capacity
    /// </summary>
    private static Tuple<double[], double[]> Dudka2018_Fig3A_BAL27862()
    {
      double[] times    = new double[] { 0,   5,   10,  15,   20,   25,
                                         50,   65,   70,   90,   95,
                                         135,  140,  155,  160,  180 };
      double[] percents = new double[] { 0.0, 0.0, 4.6, 25.1, 72.1, 83.4,
                                         90.5, 90.5, 91.9, 91.9, 94.7,
                                         94.7, 95.9, 95.9, 97.4, 97.4 };

      if (times.Length != percents.Length)
      { throw new ApplicationException("Wrong data for Dudka 2018 (Fig. 3A)"); }

      for (int i = 0; i < times.Length; i++)
      { times[i] *= 60.0; }
      return new Tuple<double[], double[]>(times, percents);
    }

    /// <summary>
    /// Compares actual anaphase timings of RPE1 cells with our simulations
    /// </summary>
    private static DataTable Dudka2018(double[] times, int[][] kmtSlices)
    {
      var res = new DataTable("Time (sec)", times);

      var dmso = Dudka2018_Fig3A_DMSO();
      res.AddColumn("In vitro RPE1 cells: DMSO (percentage)", SelectValues(dmso.Item1, dmso.Item2, times));
      var bal27862 = Dudka2018_Fig3A_BAL27862();
      res.AddColumn("In vitro RPE1 cells: BAL27862 (percentage)", SelectValues(bal27862.Item1, bal27862.Item2, times));
      res.AddColumn("In silico cells (percentage)", CalculateAnaphaseReadyFrequency(kmtSlices));
      res.Legend  = "Timings from NEB to anaphase with suppressed kinetochore binding slots";
      res.Legend += ": https://doi.org/10.1038/s41467-018-04427-x";

      return res;
    }
  }
}
