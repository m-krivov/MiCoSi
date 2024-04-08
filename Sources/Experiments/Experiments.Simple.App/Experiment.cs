using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;

namespace Experiments
{
  /// <summary>
  /// A little framework that performs some experiments and collects cell statistics
  /// If RNG seed is static, supports check points
  /// So, it my be terminated and re-launched again
  /// </summary>
  public partial class Experiment
  {
    /// <summary>
    /// Creates the empty experiment framework that will perform simulations
    /// Do not forget to enable options that you want to analyze and
    /// remember that framework uses statically defined RNG seed
    /// </summary>
    public Experiment(String solverPath)
    {
      if (!File.Exists(solverPath))
      {
        throw new ApplicationException(
            String.Format("wrong path to solver: '{0}'", new FileInfo(solverPath).FullName)
        );
      }

      _solverPath = new FileInfo(solverPath).FullName;

      TableFormat = new TableFormat();
      Analyzers = new CellAnalyzers();
      Seed = 26022021;
      BatchSize = 0;
      Logging = Console.Out;
    }

    /// <summary>
    /// Allows you to configure data that should be saved to CSV-tables
    /// </summary>
    public TableFormat TableFormat { get; private set; }

    /// <summary>
    /// Allows you to select the type of analysis to be performed
    /// You must specify at least one analyzer
    /// </summary>
    public CellAnalyzers Analyzers { get; private set; }

    /// <summary>
    /// Sets or gets new seed of RNG
    /// Negative values will be replaced by a tick-based seed
    /// </summary>
    public Int32 Seed
    {
      get { return _seed; }
      set
      {
        _seed = value;
        if (_seed < 0)
        { _seed = Environment.TickCount; }
      }
    }

    /// <summary>
    /// Gets or sets the number of cells that may be processed in parallel
    /// Use '0' to configure the batch size automatically
    /// </summary>
    public UInt32 BatchSize
    {
      get { return _batchSize; }
      set
      {
        _batchSize = value;
        if (_batchSize == 0)
        { _batchSize = (uint)Math.Max(1, Environment.ProcessorCount); }
      }
    }

    /// <summary>
    /// Wraps the 'BatchSize' property for Parallel.For
    /// </summary>
    private ParallelOptions OurParallelOptions
    {
      get
      {
        var res = new ParallelOptions();
        res.MaxDegreeOfParallelism = (int)BatchSize;
        return res;
      }
    }

    /// <summary>
    /// Sets or gets the stream that will be used for log messages
    /// By default, uses standard console
    /// </summary>
    public TextWriter Logging
    {
      get
      { return _logger; }
      set
      {
        _logger = value;
        if (_logger == null)
        { _logger = TextWriter.Null; }
      }
    }

    public String Hash()
    {
      using (var md5 = System.Security.Cryptography.MD5.Create())
      {
        StringBuilder builder = null;

        // Create hash for the solver binary
        // Someone may recompile it
        byte[] solverHash;
        using (var fs = File.OpenRead(_solverPath))
        { solverHash = md5.ComputeHash(fs); }

        // Add another hash for all options
        byte[] optionHash;
        builder = new StringBuilder();
        builder.Append(Seed);
        builder.Append(';');
        builder.Append(TableFormat.ToString());
        builder.Append(';');
        builder.Append(Analyzers.ToString());
        builder.Append(';');
        builder.Append(BatchSize);
        builder.Append(';');

        var options = Encoding.ASCII.GetBytes(builder.ToString());
        optionHash = md5.ComputeHash(options);

        // Convert two hashes to an ASCII string
        builder = new StringBuilder();
        foreach (var v in solverHash)
        { builder.Append(v.ToString("X2")); }
        foreach (var v in optionHash)
        { builder.Append(v.ToString("X2")); }

        return builder.ToString();
      }
    }

    public override int GetHashCode()
    { return Hash().GetHashCode(); }

    private IAnalyzer[] CreateAnalyzers()
    {
      List<IAnalyzer> res = new List<IAnalyzer>();
      
      if (Analyzers.Centromere)
      { res.Add(new CentromereAnalyzer(OurParallelOptions, TableFormat)); }

      if (Analyzers.KMTs)
      { res.Add(new KmtAnalyzer(Analyzers.KMTs, OurParallelOptions, TableFormat)); }

      if (Analyzers.Movement)
      { res.Add(new MovementAnalyzer(Analyzers.Movement, OurParallelOptions, TableFormat)); }

      return res.ToArray();
    }

    private readonly String _solverPath;
    private TextWriter _logger;
    private int _seed;
    private uint _batchSize;
  }
}
