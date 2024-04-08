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
    private static readonly String ID = "KMTs";

    private readonly TableFormat _tableFormat;
    private readonly Experiment.CellAnalyzers.KmtOptions _kmtOptions;

    public KmtAnalyzer(Experiment.CellAnalyzers.KmtOptions kmtOptions,
                       ParallelOptions parallelOptions,
                       TableFormat tableFormat)
      : base(parallelOptions)
    {
      _tableFormat = tableFormat;
      _kmtOptions = kmtOptions;
    }

    private static int[] GetSlice(AnalyzedCell[] acells, uint nCell, uint nPair)
    { return acells[nCell].GetIntSlice(String.Format("{0}:{1}", ID, nPair)); }

    public override void Initialize(AnalyzedCell acell, SimParams config, int layers)
    {
      var N_Cr_Total = config[SimParameter.Int.N_Cr_Total];
      for (int i = 0; i < N_Cr_Total; i++)
      {
        var slice = acell.CreateIntSlice(String.Format("{0}:{1}", ID, i), (uint)layers * 4);
        for (int j = 0; j < slice.Length; j++)
        { slice[j] = 0; }
      }
    }

    public override void Process(AnalyzedCell acell, SimParams config, Cell cell, int layer)
    {
      int sign = cell.AreSpringsBroken ? -1 : 1;
      var pairs = cell.ChromosomePairs.ToArray();
      for (int i = 0; i < pairs.Length; i++)
      {
        var pair = pairs[i];
        var slice = acell.GetIntSlice(String.Format("{0}:{1}", ID, i));
        
        int k0l = 0, k0r = 0, k1l = 0, k1r = 0;
        var c0 = pair.LeftChromosome.BoundMTs;
        foreach (var mt in c0)
        {
          if (mt.Pole.Type == PoleType.Left) { k0l += 1; }
          else { k0r += 1; }
        }

        var c1 = pair.RightChromosome.BoundMTs;
        foreach (var mt in c1)
        {
          if (mt.Pole.Type == PoleType.Left) { k1l += 1; }
          else { k1r += 1; }
        }

        slice[layer * 4 + 0] = k0l * sign;
        slice[layer * 4 + 1] = k0r;
        slice[layer * 4 + 2] = k1l;
        slice[layer * 4 + 3] = k1r;
      }
    }

    public override void Format(AnalyzedCell[] acells, SimParams config, string dir, uint idx)
    {
      var nChromatidPairs = config[SimParameter.Int.N_Cr_Total];
      var N_KMT_Max = config[SimParameter.Int.N_KMT_Max];

      // As we have too many options, these helpers reduce the number of code lines
      Action<DataTable, String> saveCsv = (table, subname) =>
      {
        table.ToCsv(Path.Combine(dir, String.Format("{0}_{1}_{2}.csv", idx, ID, subname)),
                    _tableFormat.CsvSeparator, _tableFormat.Precision);
      };
      Action<Tuple<DataTable, String>[], String> saveCsvs = (tables, subname) =>
      {
        foreach (var table in tables)
        { saveCsv(table.Item1, String.Format("{0}_{1}", subname, table.Item2)); }
      };

      int[][] slices = new int[acells.Length * nChromatidPairs][];
      for (int cell = 0; cell < acells.Length; cell++)
      {
        for (int pair = 0; pair < nChromatidPairs; pair++)
        { slices[cell * nChromatidPairs + pair] = GetSlice(acells, (uint)cell, (uint)pair); }
      }

      // Save the final tables
      if (_kmtOptions.Total)
      { saveCsv(CountTotal(acells, nChromatidPairs), "Total"); }

      if (_kmtOptions.Merotelic)
      { saveCsv(CountMerotelic(acells, nChromatidPairs), "Merotelic"); }

      if (_kmtOptions.ByTypes)
      { saveCsv(ClassifyKinetochores(acells[0].TimeTable, slices), "ByTypes"); }

      if (_kmtOptions.AnaphaseReady)
      { saveCsv(EstimateAnaphaseReadiness(acells, nChromatidPairs), "AnaphaseReady"); }

      if (_kmtOptions.Dudka2018)
      { saveCsv(Dudka2018(acells[0].TimeTable, slices), "Dudka2018_Fig3A"); }

      if (_kmtOptions.McEwen2001)
      { saveCsvs(McEwen2001(acells[0].TimeTable, slices, N_KMT_Max), "McEwen2001"); }

      if (_kmtOptions.Meraldi2004)
      { saveCsvs(Meraldi2004(acells[0].TimeTable, slices), "Meraldi2004"); }

      if (_kmtOptions.Rieder1994)
      { saveCsvs(Rieder1994(acells[0].TimeTable, slices), "Rieder1994"); }
    }
  }
}
