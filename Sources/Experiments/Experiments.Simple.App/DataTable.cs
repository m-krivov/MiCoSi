using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;

namespace Experiments
{
  /// <summary>
  /// Represents table that can be converted to CSV-format and always has the argument column
  /// </summary>
  public class DataTable : ICloneable
  {
    /// <summary>
    /// Tuple-like classes that bind column name and its values
    /// </summary>
    private interface IDataColumn : ICloneable
    {
      String Name { get; }
      int Count { get; }      
      String GetStringValue(int n, NumberFormatInfo nfi);
    }

    private abstract class GenericDataColumn<T> : IDataColumn
    {
      public String Name { get; private set; }
      public T[] Values { get; private set; }
      public int Count { get { return Values.Length; } }

      public GenericDataColumn(String name, T[] values)
      {
        Name = name;
        Values = values == null ? new T[0] : values.Clone() as T[];
      }

      public abstract String GetStringValue(int n, NumberFormatInfo nfi);
      public abstract Object Clone();
    }

    private class BoolDataColumn : GenericDataColumn<bool>
    {
      public BoolDataColumn(String name, bool[] values) : base(name, values)
      { }

      public override string GetStringValue(int n, NumberFormatInfo nfi)
      { return Values[n] ? "yes" : "no"; }

      public override Object Clone()
      { return new BoolDataColumn(Name, Values); }
    }

    private class IntDataColumn : GenericDataColumn<int>
    {
      public IntDataColumn(String name, int[] values) : base(name, values)
      { }

      public override string GetStringValue(int n, NumberFormatInfo nfi)
      { return Values[n].ToString(); }

      public override Object Clone()
      { return new IntDataColumn(Name, Values); }
    }

    private class DoubleDataColumn : GenericDataColumn<double>
    {
      public DoubleDataColumn(String name, double[] values) : base(name, values)
      { }

      public override string GetStringValue(int n, NumberFormatInfo nfi)
      { return Values[n].ToString("F", nfi); }

      public override Object Clone()
      { return new DoubleDataColumn(Name, Values); }
    }

    private class StringDataColumn : GenericDataColumn<String>
    {
      public StringDataColumn(String name, String[] values) : base(name, values)
      { }

      public override string GetStringValue(int n, NumberFormatInfo nfi)
      { return Values[n]; }

      public override Object Clone()
      { return new StringDataColumn(Name, Values); }
    }

    /// <summary>
    /// The main implementation of data table
    /// </summary>
    private readonly IDataColumn _argumentColumn;
    private readonly Dictionary<String, IDataColumn> _valueColumns;

    private DataTable(IDataColumn argumentColumn)
    {
      _argumentColumn = argumentColumn.Clone() as IDataColumn;
      _valueColumns = new Dictionary<String, IDataColumn>();
      Legend = String.Empty;
    }

    public DataTable(String argumentColumnName, double[] argumentValues)
      : this(new DoubleDataColumn(argumentColumnName, argumentValues))
    { }

    public DataTable(String argumentColumnName, String[] argumentValues)
      : this(new StringDataColumn(argumentColumnName, argumentValues))
    { }

    private void AddColumn(IDataColumn column)
    {
      if (column.Count != RowNumber)
      {
        throw new ApplicationException(
          String.Format("Wrong number of rows, '{0}' elements per column are expected", RowNumber));
      }
      
      IDataColumn tmp;
      if (_valueColumns.TryGetValue(column.Name, out tmp))
      {
        throw new ApplicationException(
          String.Format("Column with name '{0}' is already exist", column.Name)
          );
      }

      _valueColumns.Add(column.Name, column);
    }

    public void AddColumn(String name, bool[] values)
    { AddColumn(new BoolDataColumn(name, values)); }

    public void AddColumn(String name, int[] values)
    { AddColumn(new IntDataColumn(name, values)); }

    public void AddColumn(String name, double[] values)
    { AddColumn(new DoubleDataColumn(name, values)); }

    public String Legend { get; set; }
    public int RowNumber { get { return _argumentColumn.Count; } }

    public void ToCsv(String filename, char separator, int precision)
    {
      var format = CultureInfo.InvariantCulture.NumberFormat.Clone() as NumberFormatInfo;
      format.NumberDecimalDigits = precision;

      using (var sw = new StreamWriter(filename))
      {
        var arg = _argumentColumn;
        var values = _valueColumns.Values.ToArray();

        if (!String.IsNullOrEmpty(Legend))
        { sw.WriteLine(Legend + separator); }
        else
        { sw.WriteLine("MiCoSi simulation data set"); }

        sw.Write(arg.Name + separator);
        foreach (var val in values)
        { sw.Write(val.Name + separator); }
        sw.WriteLine();

        for (int i = 0; i < RowNumber; i++)
        {
          sw.Write(arg.GetStringValue(i, format) + separator);
          for (int j = 0; j < values.Length; j++)
          { sw.Write(values[j].GetStringValue(i, format) + separator); }
          sw.WriteLine();
        }
      } // using sw
    }

    public Object Clone()
    {
      DataTable res = new DataTable(_argumentColumn.Clone() as IDataColumn);
      foreach (var rec in _valueColumns)
      {
        var column = rec.Value;
        res._valueColumns.Add(column.Name, column.Clone() as IDataColumn);
      }
      res.Legend = Legend;
      return res;
    }
  }
}
