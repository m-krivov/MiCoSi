using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading.Tasks;

namespace Experiments
{
  /// <summary>
  /// Contains some arbitrary information that was extracted from a virtual cell
  /// We use these data to construct the final CSV tables
  /// </summary>
  public class AnalyzedCell
  {
    /// <summary>
    /// Gets the RNG seed that was used to initialize the simulation
    /// </summary>
    public uint Seed { get; private set; }

    /// <summary>
    /// Returns a table that maps record index to its time
    /// It always contains at least one element
    /// </summary>
    public double[] TimeTable { get; private set; }

    /// <summary>
    /// Creates and empty instance without data slices
    /// </summary>
    public AnalyzedCell(uint seed)
    {
      Seed = seed;
      _boolSlices = new Dictionary<String, bool[]>();
      _intSlices = new Dictionary<String, int[]>();
      _doubleSlices = new Dictionary<String, double[]>();
    }

    /// <summary>
    /// Creates time table for this cell (defines the correspondence between records and times)
    /// Each analyzed cell must have such table, you are allowed to create only one table
    /// </summary>
    public void CreateTimeTable(double[] timeTable)
    {
      if (TimeTable != null)
      { throw new ApplicationException("Time table is already constructed"); }

      TimeTable = timeTable.Clone() as double[];
    }

    private static T[] CreateSlice<T>(Dictionary<String, T[]> slices, String name, uint size)
    {
      T[] res;
      if (slices.TryGetValue(name, out res))
      { throw new ApplicationException("Such slice is already created"); }

      res = new T[size];
      slices.Add(name, res);
      return res;
    }

    /// <summary>
    /// Creates a named slice (array) that contains
    /// some aggregated information about the analyzing cell
    /// </summary>
    public bool[] CreateBoolSlice(String name, uint size)
    { return CreateSlice<bool>(_boolSlices, name, size); }

    /// <summary>
    /// Creates a named slice (array) that contains
    /// some aggregated information about the analyzing cell
    /// </summary>
    public int[] CreateIntSlice(String name, uint size)
    { return CreateSlice<int>(_intSlices, name, size); }

    /// <summary>
    /// Creates a named slice (array) that contains
    /// some aggregated information about the analyzing cell
    /// </summary>
    public double[] CreateDoubleSlice(String name, uint size)
    { return CreateSlice<double>(_doubleSlices, name, size); }

    private static T[] GetSlice<T>(Dictionary<String, T[]> slices, String name)
    {
      T[] res;
      if (!slices.TryGetValue(name, out res))
      { throw new ApplicationException("Slice with such name is not created"); }
      return res;
    }

    /// <summary>
    /// Returns the already created slice (array) that is used to store some information
    /// </summary>
    public bool[] GetBoolSlice(String name)
    { return GetSlice<bool>(_boolSlices, name); }

    /// <summary>
    /// Returns the already created slice (array) that is used to store some information
    /// </summary>
    public int[] GetIntSlice(String name)
    { return GetSlice<int>(_intSlices, name); }

    /// <summary>
    /// Returns the already created slice (array) that is used to store some information
    /// </summary>
    public double[] GetDoubleSlice(String name)
    { return GetSlice<double>(_doubleSlices, name); }

    /// <summary>
    /// Saves the time table and all slices to a file
    /// </summary>
    public void Save(String filename)
    {
      using (var fs = File.OpenWrite(filename))
      {
        BinaryFormatter serializer = new BinaryFormatter();
        serializer.Serialize(fs, Seed);
        serializer.Serialize(fs, TimeTable);
        serializer.Serialize(fs, _boolSlices);
        serializer.Serialize(fs, _intSlices);
        serializer.Serialize(fs, _doubleSlices);
      }
    }

    /// <summary>
    /// Tries to load the time table and slices from a file
    /// </summary>
    static public bool TryLoad(String filename, out AnalyzedCell res)
    {
      try
      {
        using (var fs = File.OpenRead(filename))
        {
          var serializer = new BinaryFormatter();
          res = new AnalyzedCell((uint)serializer.Deserialize(fs));
          res.CreateTimeTable((double[])serializer.Deserialize(fs));
          res._boolSlices = (Dictionary<String, bool[]>)serializer.Deserialize(fs);
          res._intSlices = (Dictionary<String, int[]>)serializer.Deserialize(fs);
          res._doubleSlices = (Dictionary<String, double[]>)serializer.Deserialize(fs);
        }
        return true;
      }
      catch (Exception)
      { 
        res = null;
        return false;
      }
    }

    private Dictionary<String, bool[]> _boolSlices;
    private Dictionary<String, int[]> _intSlices;
    private Dictionary<String, double[]> _doubleSlices;
  }
}
