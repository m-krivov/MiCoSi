using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Numerics;
using MiCoSi;

namespace Experiments
{
  /// <summary>
  /// Specifies configuration of a single experiment
  /// You may consider it as a some hypthesis that must be verified
  /// </summary>
  public class Specification
  {
    /// <summary>
    /// Creates ready-to-use configuration for default cell (without chromosomes)
    /// </summary>
    public Specification()
    {
      Name = "Default";
      Cells = 1;
      MovePoles = true;
      Config = null;
      _chromosomes = null;
    }

    /// <summary>
    /// Some user-friendly name that will be used to differ this specification from others
    /// </summary>
    public String Name { get; set; }

    /// <summary>
    /// Defines how many cells must be analyzed
    /// </summary>
    public UInt32 Cells
    {
      get
      { return _cells; }
      set
      {
        if (value == 0)
        { throw new ApplicationException("cannot simulate 0 cells"); }
        _cells = value;
      }
    }

    /// <summary>
    /// Poles must be moving (prometaphase) or statically initialized (early metaphase)
    /// </summary>
    public bool MovePoles { get; set; }

    /// <summary>
    /// Sets or gets configuration of the simulating cell
    /// </summary>
    public SimParams Config
    {
      get { return _config; }
      set
      {
        _config = value;
        if (_config == null)
        { _config = new SimParams(); }
      }
    }

    /// <summary>
    /// Sets or gets the list of chromosome positions and orientations
    /// For example, '{0.0f, 1e-6f, 0.0f}, {Math.Pi / 2, 0.0f, 0.0f}' means that
    /// sister chromatids must be shifted by OY and rotated around OX
    /// </summary>
    public List<Tuple<Vector3, Vector3>> Chromosomes
    {
      get { return _chromosomes; }
      set
      {
        _chromosomes = value;
        if (_chromosomes == null)
        { _chromosomes = new List<Tuple<Vector3, Vector3>>(); }
      }
    }

    public override string ToString()
    {
      return Name;
    }

    public String Hash()
    {
      var builder = new StringBuilder();
      builder.Append(Name);
      builder.Append(';');
      builder.Append(Cells);
      builder.Append(';');
      builder.Append(MovePoles);
      builder.Append(';');
      if (_chromosomes != null)
      {
        foreach (var chr in _chromosomes)
        {
          builder.Append('{');
          builder.Append(chr.Item1.ToString());
          builder.Append(',');
          builder.Append(chr.Item2.ToString());
          builder.Append('}');
        }
      }
      builder.Append(';');
      builder.Append(_config.ExportAsProps());

      var data = Encoding.ASCII.GetBytes(builder.ToString());
      byte[] hash = null;
      using (var md5 = System.Security.Cryptography.MD5.Create())
      { hash = md5.ComputeHash(data); }

      builder = new StringBuilder();
      foreach (var v in hash)
      { builder.Append(v.ToString("X2")); }

      return builder.ToString();
    }

    public override int GetHashCode()
    { return Hash().GetHashCode(); }

    private uint _cells;
    private SimParams _config;
    private List<Tuple<Vector3, Vector3>> _chromosomes;
  }
}
