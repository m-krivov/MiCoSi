using System;

namespace Experiments
{
  internal partial class MovementAnalyzer : BasicAnalyzer
  {
    private static Tuple<double[], double[]> Magidson2011_Fig6A_Orientation()
    {
      double[] times  = new double[] { 0,    1,    2,    3,    4,
                                       5,    6,    7,    8,    9,
                                       10,   11,   12,   13,   14,
                                       15 };
      double[] angles = new double[] { 56.5, 54.5, 50.6, 44.3, 37.8,
                                       32.7, 28.4, 24.5, 20.1, 17.7,
                                       16.9, 16.0, 15.3, 15.0, 14.8,
                                       14.6 };
      if (times.Length != angles.Length ||
          times.Length != 16)
      { throw new ApplicationException("Wrong data from Magidson 2011 (Fig 6A)"); }

      for (int i = 0; i < times.Length; i++)
      { times[i] *= 60.0; }
      return new Tuple<double[], double[]>(times, angles);
    }

    private DataTable Magidson2011(double[] times, double[] angleSlice)
    {
      double[] inSilicoColumn = new double[times.Length];
      for (int i = 0; i < inSilicoColumn.Length; i++)
      { inSilicoColumn[i] = angleSlice[2 * i]; }
      
      var inVitro = Magidson2011_Fig6A_Orientation();
      var inVitroColumn = SelectValues(inVitro.Item1, inVitro.Item2, times);

      var res = new DataTable("Time (seconds)", times);
      res.AddColumn("In vitro RPE1 cells (degrees)", inVitroColumn);
      res.AddColumn("In silico cells (degrees)", inSilicoColumn);
      res.Legend  = "The angle between pole-to-pole line and centromere";
      res.Legend += ": http://dx.doi.org/10.1016/j.cell.2011.07.012";
      return res;
    }
  }
}
