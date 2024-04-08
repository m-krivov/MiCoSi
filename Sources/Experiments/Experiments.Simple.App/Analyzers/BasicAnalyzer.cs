using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MiCoSi;

namespace Experiments
{
  /// <summary>
  /// Auxiliary class that implements some common routines wanted by other analyzers
  /// </summary>
  internal abstract class BasicAnalyzer : IAnalyzer
  {
    private readonly ParallelOptions _options;

    protected BasicAnalyzer(ParallelOptions options)
    { _options = options == null ? new ParallelOptions() : options; }

    protected delegate T[] ColumnPerCellProvider<T>(uint nCell);
    protected delegate T[] ColumnPerChromatidPairProvider<T>(uint nCell, uint nChrPair);
    protected delegate T[] ColumnPerKinetochoreProvider<T>(uint nCell, uint nChrPair, uint nKin);

    protected DataTable ConstructTable<T>(TableFormat.SingleValueColumns tableFormat, double[] timeColumn,
                                          ColumnPerCellProvider<T> provider,
                                          Func<T, double> toDouble,
                                          Action<DataTable, String, T[]> appendColumn,
                                          int nCells)
    {
      return ConstructUniTable<T>(tableFormat, timeColumn,
                                  (uint nCell, uint nChrPair, uint nKin) => { return provider(nCell); },
                                  toDouble, appendColumn, nCells, 1, false);
    }

    protected DataTable ConstructTable<T>(TableFormat.SingleValueColumns tableFormat, double[] timeColumn,
                                          ColumnPerChromatidPairProvider<T> provider,
                                          Func<T, double> toDouble,
                                          Action<DataTable, String, T[]> appendColumn,
                                          int nCells, int nChromatidPairs)
    {
      return ConstructUniTable<T>(tableFormat, timeColumn,
                                  (uint nCell, uint nChrPair, uint nKin) => { return provider(nCell, nChrPair); },
                                  toDouble, appendColumn, nCells, nChromatidPairs, false);
    }

    protected DataTable ConstructTable<T>(TableFormat.SingleValueColumns tableFormat, double[] timeColumn,
                                          ColumnPerKinetochoreProvider<T> provider,
                                          Func<T, double> toDouble,
                                          Action<DataTable, String, T[]> appendColumn,
                                          int nCells, int nChromatidPairs)
    {
      return ConstructUniTable<T>(tableFormat, timeColumn,
                                  (uint nCell, uint nChrPair, uint nKin) => { return provider(nCell, nChrPair, nKin); },
                                  toDouble, appendColumn, nCells, nChromatidPairs, true);
    }

    private DataTable ConstructUniTable<T>(TableFormat.SingleValueColumns tableFormat, double[] timeColumn,
                                           ColumnPerKinetochoreProvider<T> provider,
                                           Func<T, double> toDouble,
                                           Action<DataTable, String, T[]> appendColumn,
                                           int nCells, int nChromatidPairs, bool bothKinetochores)
    {
      var table = new DataTable("Time (seconds)", timeColumn);
      int nKinetochores = bothKinetochores ? 2 : 1;

      // If no columns/rows are required, then there is nothing to do :)
      if (!tableFormat.MinValue && !tableFormat.MaxValue && !tableFormat.Average &&
          !tableFormat.Dispersion && !tableFormat.Median && !tableFormat.Values)
      { return table; }
      
      if (nCells * nChromatidPairs == 0)
      { return table; }

      // Extract all columns to the row-based array that is friendly for L1/L2 caches
      // Requires much more memory but it should provide better scalability
      var columns = new T[nCells * nChromatidPairs * nKinetochores][];
      int rowAlignment = ((nCells * nChromatidPairs * nKinetochores + 15) / 16) * 16;
      var rowBasedData = new T[rowAlignment * timeColumn.Length];
      Parallel.For(0, nCells, _options, (cell) =>
      {
        for (int pair = 0; pair < nChromatidPairs; pair++)
        {
          for (int kin = 0; kin < nKinetochores; kin++)
          {
            var valueColumn = provider((uint)cell, (uint)pair, (uint)kin);
            if (valueColumn.Length != timeColumn.Length)
            { throw new ApplicationException("Different values of the 'T_End' parameter are not supported"); }

            int offset = (cell * nChromatidPairs + pair) * nKinetochores + kin;
            columns[offset] = valueColumn;

            int step = rowAlignment;
            for (int rec = 0; rec < valueColumn.Length; rec++)
            { rowBasedData[offset + rec * step] = valueColumn[rec]; }
          }
        }
      });

      // First pass: gather metrics for min/max and average (if needed)
      T[] minValue = null, maxValue = null;
      double[] aveValue = null;
      if (tableFormat.MinValue || tableFormat.MaxValue ||
          tableFormat.Average || tableFormat.Dispersion)
      {
        minValue = new T[timeColumn.Length];
        maxValue = new T[timeColumn.Length];
        aveValue = new double[timeColumn.Length];

        Parallel.For(0, timeColumn.Length, _options, (row) =>
        {
          T minT = rowBasedData[row * rowAlignment];
          T maxT = minT;
          double min = Double.MaxValue, max = Double.MinValue, ave = 0.0;
          int notNaNs = 0;

          for (int col = 0; col < columns.Length; col++)
          {
            double value = toDouble(rowBasedData[row * rowAlignment + col]);
            if (value != Double.NaN)
            {
              if (value < min)
              {
                min = value;
                minT = rowBasedData[row * rowAlignment + col];
              }
              if (value > max)
              {
                max = value;
                maxT = rowBasedData[row * rowAlignment + col];
              }
              ave += value;
              notNaNs += 1;
            }
          }

          minValue[row] = minT;
          maxValue[row] = maxT;
          aveValue[row] = notNaNs > 0 ? ave / notNaNs : Double.NaN;
        });
      }

      // Second pass: calculate dispersion (if needed)
      double[] dispersionValue = null;
      if (tableFormat.Dispersion)
      {
        dispersionValue = new double[timeColumn.Length];
        Parallel.For(0, timeColumn.Length, _options, (row) =>
        {
          double variance = 0;
          double mean = aveValue[row];
          int notNaNs = 0;

          for (int col = 0; col < columns.Length; col++)
          {
            double val = toDouble(rowBasedData[row * rowAlignment + col]);
            if (val != Double.NaN)
            {
              val = val - mean;
              variance += val * val;
              notNaNs += 1;
            }
          }

          dispersionValue[row] = notNaNs > 0 ? Math.Sqrt(variance / notNaNs) : Double.NaN;
        });
      }

      // The third pass: calculate median
      double[] medianValue = null;
      if (tableFormat.Median)
      {
        medianValue = new double[timeColumn.Length];
        Parallel.For(0, timeColumn.Length, _options, (row) =>
        {
          var buf = new double[nCells * nChromatidPairs * nKinetochores];
          int notNaNs = 0;
          for (int col = 0; col < columns.Length; col++)
          {
            var value = toDouble(rowBasedData[rowAlignment * row + col]);
            if (col != Double.NaN)
            { buf[notNaNs++] = value; }
          }
          medianValue[row] = notNaNs == 0 ? Double.NaN : MyMath.Median(buf, 0, notNaNs);
        });
      }

      // Add columns to table
      if (tableFormat.MinValue)
      { appendColumn(table, "Min", minValue); }
      if (tableFormat.MaxValue)
      { appendColumn(table, "Max", maxValue); }
      if (tableFormat.Average)

      { table.AddColumn("Average", aveValue); }
      if (tableFormat.Median)
      { table.AddColumn("Median", medianValue); }
      if (tableFormat.Dispersion)
      { table.AddColumn("Dispersion", dispersionValue); }

      var sbuilder = new StringBuilder();
      sbuilder.Append("Cell #{0}");
      if (nChromatidPairs > 1)
      { sbuilder.Append( "Pair #{1}"); }
      if (nKinetochores > 1)
      { sbuilder.Append(" Kinetochore #{2}"); }
      var columnFormat = sbuilder.ToString();

      if (tableFormat.Values)
      {
        for (int cell = 0; cell < nCells; cell++)
        {
          for (int pair = 0; pair < nChromatidPairs; pair++)
          {
            for (int kin = 0; kin < nKinetochores; kin++)
            {
              appendColumn(table, String.Format(columnFormat, cell, pair, kin),
                           columns[(cell * nChromatidPairs + pair) * nKinetochores + kin]);
            }
          }
        }
      }
      
      return table;
    }

    /// <summary>
    /// Builds histogram, 'labels' must cover all indices from 'columns'
    /// Optionally, groups rows by 'groupSize' elements and converts their values to percents
    /// </summary>
    protected DataTable ConstructHistogram(Tuple<String[], String> labels,
                                           Tuple<Dictionary<int, int>, String>[] columns,
                                           int groupSize, bool asPercents)
    {
      var nCols = columns.Length;
      var nRows = labels.Item1.Length;
      if (groupSize < 1 || nRows < 1 || labels.Item1.Length != nRows)
      { throw new ApplicationException("Wrong data for histogram"); }

      // To store data as a 'DataTable', we must extract it to arrays
      String[] labelsAsArray = labels.Item1.Clone() as String[];
      int[][] columnsAsArrays = new int[nCols][];
      int[] summedColumns = new int[nCols];
      for (int i = 0; i < nCols; i++)
      {
        var curColumn = new int[nRows];
        columnsAsArrays[i] = curColumn;
        foreach (var rec in columns[i].Item1)
        {
          if (rec.Key < 0 || rec.Key >= nRows)
          {
            throw new ApplicationException(
              String.Format("{0}-th column contains element that is not covered by label", i)
            );
          }
          curColumn[rec.Key] = rec.Value;
          summedColumns[i] += rec.Value;
        }
      }

      // Eliminate empty rows by defining the '[lo, hi)' range with non-zero elements
      {
        bool IsRowEmpty(int row)
        {
          bool empty = true;
          for (int col = 0; col < nCols; col++)
          {
            if (columnsAsArrays[col][row] != 0)
            {
              empty = false;
              break;
            }
          }
          return empty;
        }

        int lo = 0, hi = nRows;
        while (hi > lo + 1 && IsRowEmpty(hi - 1))
        { hi -= 1; }

        while (lo + 1 < hi && IsRowEmpty(lo))
        { lo += 1; }

        if (lo != 0 || hi != nRows)
        {
          var newLabels = new String[hi - lo];
          Array.Copy(labelsAsArray, lo, newLabels, 0, hi - lo);
          labelsAsArray = newLabels;

          for (int i = 0; i < nCols; i++)
          {
            var tmp = new int[hi - lo];
            Array.Copy(columnsAsArrays[i], lo, tmp, 0, hi - lo);
            columnsAsArrays[i] = tmp;
          }

          nRows = hi - lo;
        }
      }

      // Group rows and update their labels (if needed)
      if (groupSize > 1)
      {
        nRows = (nRows + groupSize - 1) / groupSize;
        var newLabels  = new String[nRows];
        var newColumns = new int[nCols][];
        for (int i = 0; i < nCols; i++)
        { newColumns[i] = new int[nRows]; }

        for (int i = 0; i < nRows; i++)
        {
          var idx1 = i * groupSize;
          var idx2 = Math.Min((i + 1) * groupSize, labelsAsArray.Length) - 1;
          if (idx1 != idx2)
          { newLabels[i] = String.Format("[{0} .. {1}]", labelsAsArray[idx1], labelsAsArray[idx2]); }
          else
          { newLabels[i] = labelsAsArray[idx1]; }

          for (int col = 0; col < nCols; col++)
          {
            for (int row = i * groupSize;
                 row < Math.Min((i + 1) * groupSize, columnsAsArrays[col].Length);
                 row++)
            { newColumns[col][i] += columnsAsArrays[col][row]; }
          }
        } // for i

        labelsAsArray = newLabels;
        columnsAsArrays = newColumns;
      } // if groupSize > 1

      // Finally, create the resulting table
      var res = new DataTable(labels.Item2, labelsAsArray);
      for (int col = 0; col < nCols; col++)
      {
        if (asPercents)
        {
          var buf = new double[nRows];
          var cur = columnsAsArrays[col];
          var sum = summedColumns[col];
          for (int row = 0; row < nRows; row++)
          { buf[row] = sum == 0 ? Double.NaN : (double)cur[row] / sum * 100; }
          res.AddColumn(columns[col].Item2, buf);
        }
        else
        { res.AddColumn(columns[col].Item2, columnsAsArrays[col]); }
      }

      return res;
    }

    /// <summary>
    /// Aligns some coarse experimental data by the time scale of our simulation, and vice versa
    /// </summary>
    protected static double[] SelectValues(double[] oldTimes, double[] oldValues, double[] newTimes)
    {
      if (oldTimes.Length != oldValues.Length || newTimes.Length < 2 || oldTimes.Length < 2)
      { throw new ApplicationException("wrong size of data"); }
      var res = new double[newTimes.Length];

      // Skip head
      int point = 0;
      while (point < newTimes.Length && newTimes[point] < oldTimes[0])
      { res[point++] = Double.NaN; }

      // Linearly interpolate body
      int layer = 0;
      while (point < newTimes.Length)
      {
        while (layer + 1 < oldTimes.Length && oldTimes[layer + 1] < newTimes[point])
        { layer += 1; }

        if (layer + 1 < oldTimes.Length)
        {
          double t1 = oldTimes[layer],  t2 = oldTimes[layer + 1];
          double v1 = oldValues[layer], v2 = oldValues[layer + 1];
          double w = (newTimes[point] - t1) / (t2 - t1);
          res[point] = v1 * (1.0 - w) + v2 * w;
          point += 1;
        }
        else
        { break; }
      }

      // Cut tail
      while (point < newTimes.Length)
      { res[point++] = Double.NaN; }

      return res;
    }

    public abstract void Format(AnalyzedCell[] acells, SimParams config, string dir, uint idx);
    public abstract void Initialize(AnalyzedCell acell, SimParams config, int layers);
    public abstract void Process(AnalyzedCell acell, SimParams config, Cell cell, int layer);
  }
}
