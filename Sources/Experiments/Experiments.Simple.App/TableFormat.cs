using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;

namespace Experiments
{
  /// <summary>
  /// The results of simulations are tables, a lot of tables!
  /// So, it is not a bad idea to customize them for the proposed usage
  /// </summary>
  public class TableFormat
  {
    /// <summary>
    /// If analyzer generates a single value, defines columns that we want to save as CSV table
    /// The similar calculations may be performed by MS Excel or OO.org Calc
    /// But, if we simulate thousands of cells, it saves disk space and your time
    /// </summary>
    public class SingleValueColumns
    {
      public bool MinValue { get; set; }

      public bool MaxValue { get; set; }

      public bool Average { get; set; }

      public bool Dispersion { get; set; }

      public bool Median { get; set; }

      public bool Values { get; set; }

      public SingleValueColumns()
      {
        MinValue = false;
        MaxValue = false;
        Average = true;
        Dispersion = false;
        Median = false;
        Values = true;
      }

      public override string ToString()
      {
        StringBuilder sb = new StringBuilder();

        sb.Append('{');
        sb.Append(MinValue);
        sb.Append(',');
        sb.Append(MaxValue);
        sb.Append(',');
        sb.Append(Average);
        sb.Append(',');
        sb.Append(Dispersion);
        sb.Append(',');
        sb.Append(Median);
        sb.Append(',');
        sb.Append(Values);
        sb.Append('}');

        return sb.ToString();
      }

      public override int GetHashCode()
      { return ToString().GetHashCode(); }
    }

    /// <summary>
    /// Defines the symbol that separates cells in a CSV table
    /// By default, comma is used
    /// But the Russian version of MS Excel wants semicolon
    /// </summary>
    public char CsvSeparator { get; set; }

    /// <summary>
    /// Defines how many digits must be printed after dot
    /// </summary>
    public int Precision { get; set; }

    /// <summary>
    /// Specifies columns that we want to save
    /// </summary>
    public SingleValueColumns Columns { get; private set; }

    public TableFormat()
    {
      CsvSeparator = ',';
      Precision = 2;
      Columns = new SingleValueColumns();
    }

    public override string ToString()
    {
      var builder = new StringBuilder();

      builder.Append('{');
      builder.Append(CsvSeparator);
      builder.Append(';');
      builder.Append(Precision);
      builder.Append(';');
      builder.Append(Columns.ToString());
      builder.Append('}');

      return builder.ToString();
    }

    public override int GetHashCode()
    { return ToString().GetHashCode(); }
  }
}
