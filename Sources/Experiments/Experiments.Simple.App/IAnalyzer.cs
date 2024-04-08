using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MiCoSi;

namespace Experiments
{
  /// <summary>
  /// An essence that can process a single cell simulation and somehow format the results
  /// </summary>
  public interface IAnalyzer
  {
    /// <summary>
    /// Allocates necessary arrays in 'acell'
    /// </summary>
    void Initialize(AnalyzedCell acell, SimParams config, int layers);

    /// <summary>
    /// Collects the required information about 'cell', stores it to 'acell'
    /// </summary>
    void Process(AnalyzedCell acell, SimParams config, Cell cell, int layer);

    /// <summary>
    /// Converts the previously gathered information to a file with some user-friendly format
    /// The 'idx' value defines the ordinal number of analyzer and may be used in filenames
    /// </summary>
    void Format(AnalyzedCell[] acells, SimParams config, String dir, uint idx);
  }
}
