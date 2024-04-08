using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using MiCoSi;

namespace Experiments
{
  internal partial class MovementAnalyzer : BasicAnalyzer
  {
    private static readonly String ID = "Movement";

    private readonly TableFormat _tableFormat;
    private readonly Experiment.CellAnalyzers.MovementOptions _movementOptions;

    public MovementAnalyzer(Experiment.CellAnalyzers.MovementOptions movementOptions,
                            ParallelOptions parallelOptions,
                            TableFormat tableFormat)
      : base(parallelOptions)
    {
      _tableFormat = tableFormat;
      _movementOptions = movementOptions;
    }

    private static double[] GetAngleSlice(AnalyzedCell[] acells, uint nCell, uint nPair)
    { return acells[nCell].GetDoubleSlice(String.Format("{0}:{1}:angles", ID, nPair)); }

    public override void Initialize(AnalyzedCell acell, SimParams config, int layers)
    {
      var N_Cr_Total = config[SimParameter.Int.N_Cr_Total];
      for (int i = 0; i < N_Cr_Total; i++)
      {
        var slice = acell.CreateDoubleSlice(String.Format("{0}:{1}:angles", ID, i), (uint)layers * 2);
        for (int j = 0; j < slice.Length; j++)
        { slice[j] = Double.NaN; }
      }
    }

    public override void Process(AnalyzedCell acell, SimParams config, Cell cell, int layer)
    {
      if (cell.AreSpringsBroken)
      { throw new ApplicationException("anaphase B is not supported by this analyzer"); }

      var pairs = cell.ChromosomePairs.ToArray();
      for (int i = 0; i < pairs.Length; i++)
      {
        // Calculate unit vector for pole-to-pole direction
        var poleToPole = new Vector3(cell.GetPole(PoleType.Left).Position.X - cell.GetPole(PoleType.Right).Position.X,
                                     cell.GetPole(PoleType.Left).Position.Y - cell.GetPole(PoleType.Right).Position.Y,
                                     cell.GetPole(PoleType.Left).Position.Z - cell.GetPole(PoleType.Right).Position.Z);
        var poleDistance = poleToPole.Length();
        if (poleDistance < 0.001f * 1e-6f)
        { poleToPole = new Vector3(-1.0f, 0.0f, 0.0f); }
        else
        { poleToPole = poleToPole / poleDistance; }

        // And two more unit vectors: aligned with centromere and arms
        var chr0 = pairs[i].LeftChromosome;
        var centromere = new Vector3((float)-chr0.Orientation[0, 0],
                                     (float)-chr0.Orientation[1, 0],
                                     (float)-chr0.Orientation[2, 0]);
        centromere = centromere / centromere.Length();

        var arms = new Vector3((float)-chr0.Orientation[0, 1],
                               (float)-chr0.Orientation[1, 1],
                               (float)-chr0.Orientation[2, 1]);
        arms = arms / arms.Length();

        // Based on these unit vectors, calculate the actual angles
        // They must not exceed 90 degrees due to the cell symmetry
        var slice = acell.GetDoubleSlice(String.Format("{0}:{1}:angles", ID, i));
        var centromereAngle = Math.Acos(Math.Min(1.0, Math.Max(-1.0, Vector3.Dot(poleToPole, centromere))));
        if (centromereAngle > Math.PI / 2) { centromereAngle = Math.PI - centromereAngle; }
        slice[layer * 2 + 0] = centromereAngle / Math.PI * 180;

        var armAngle = Math.Acos(Math.Min(1.0, Math.Max(-1.0, Vector3.Dot(poleToPole, arms))));
        if (armAngle > Math.PI / 2) { armAngle = Math.PI - armAngle; }
        slice[layer * 2 + 1] = armAngle / Math.PI * 180;
      }
    }

    private Tuple<DataTable, DataTable> MeasureAngles(AnalyzedCell[] acells, int nPairs)
    {
      Func<int, DataTable> createTable = (int offset) =>
      {
        return ConstructTable<double>(_tableFormat.Columns, acells[0].TimeTable,
                                      (uint nCell, uint nPair) =>
                                      {
                                        var slice = GetAngleSlice(acells, nCell, nPair);
                                        var column = new double[slice.Length / 2];
                                        for (int i = 0; i < column.Length; i++)
                                        { column[i] = slice[i * 2 + offset]; }
                                        return column;
                                      },
                                      (value) => value,
                                      (tbl, name, values) => tbl.AddColumn(name, values),
                                      acells.Length, nPairs);
      };
      
      var centromereTable = createTable(0);
      centromereTable.Legend = "The angle between direction of centromere and pole-to-pole line (in degrees)";

      var armTable = createTable(1);
      armTable.Legend = "The angle between direction of chromatids' arms and pole-to-pole line (in degrees)";
      
      return new Tuple<DataTable, DataTable>(centromereTable, armTable);
    }

    public override void Format(AnalyzedCell[] acells, SimParams config, string dir, uint idx)
    {
      var nPairs = config[SimParameter.Int.N_Cr_Total];
      Action<DataTable, String> saveCsv = (table, subname) =>
      {
        table.ToCsv(Path.Combine(dir, String.Format("{0}_{1}_{2}.csv", idx, ID, subname)),
                    _tableFormat.CsvSeparator, _tableFormat.Precision);
      };
      var times = acells[0].TimeTable;
      var avgAngleSlice = new double[times.Length * 2];
      for (int nCell = 0; nCell < acells.Length; nCell++)
      {
        for (int nPair = 0; nPair < nPairs; nPair++)
        {
          var slice = GetAngleSlice(acells, (uint)nCell, 0);
          for (int i = 0; i < avgAngleSlice.Length; i++)
          { avgAngleSlice[i] += slice[i]; }
        }
      }
      for (int i = 0; i < avgAngleSlice.Length; i++)
      { avgAngleSlice[i] /= nPairs * acells.Length; }

      if (_movementOptions.Orientation)
      {
        var tables = MeasureAngles(acells, nPairs);
        saveCsv(tables.Item1, "CentromereAngle");
        saveCsv(tables.Item2, "ArmAngle");
      }

      if (_movementOptions.Magidson2011)
      { saveCsv(Magidson2011(acells[0].TimeTable, avgAngleSlice), "Magidson2011"); }
    }
  }
}
