using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using MiCoSi;

namespace Experiments
{
  /// <summary>
  /// Measures the actual length of centromere
  /// </summary>
  internal class CentromereAnalyzer : BasicAnalyzer
  {
    private readonly String ID = "Centromere";
    private readonly TableFormat _tableFormat;

    public CentromereAnalyzer(ParallelOptions options, TableFormat tableFormat)
      : base(options)
    { _tableFormat = tableFormat; }

    public override void Initialize(AnalyzedCell acell, SimParams config, int layers)
    {
      var N_Cr_Total = config[SimParameter.Int.N_Cr_Total];
      for (int i = 0; i < N_Cr_Total; i++)
      {
        var slice = acell.CreateDoubleSlice(String.Format("{0}:{1}", ID, i), (uint)layers);
        for (int j = 0; j < slice.Length; j++)
        { slice[j] = Double.NaN; }
      }
    }

    public override void Process(AnalyzedCell acell, SimParams config, Cell cell, int layer)
    {
      if (!cell.AreSpringsBroken)
      {
        var pairs = cell.ChromosomePairs.ToArray();
        for (int i = 0; i < pairs.Length; i++)
        {
          var slice = acell.GetDoubleSlice(String.Format("{0}:{1}", ID, i));
          if (layer >= slice.Length)
          { throw new ApplicationException("Unexpected time layer"); }

          slice[layer] = pairs[i].Spring.Length * 1e6;
        }
      }
    }

    public override void Format(AnalyzedCell[] acells, SimParams config, String dir, uint idx)
    {
      var table = ConstructTable<double>(_tableFormat.Columns, acells[0].TimeTable,
                                         (uint nCell, uint nChr) => acells[nCell].GetDoubleSlice(String.Format("{0}:{1}", ID, nChr)),
                                         (double value) => { return value; },
                                         (DataTable tbl, String caption, double[] values) => tbl.AddColumn(caption, values),
                                         acells.Length, config[SimParameter.Int.N_Cr_Total]);       // in parallel
      table.Legend = "The actual length of centromere (um)";
      table.ToCsv(Path.Combine(dir, String.Format("{0}_{1}_Tension.csv", idx, ID)),
                  _tableFormat.CsvSeparator, _tableFormat.Precision);
    }
  }
}
