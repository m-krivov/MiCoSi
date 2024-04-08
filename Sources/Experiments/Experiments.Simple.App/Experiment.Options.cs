using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Experiments
{
  public partial class Experiment
  {
    /// <summary>
    /// Specifies what type of measurement to perform
    /// </summary>
    public class CellAnalyzers
    {
      public class CentromereOptions
      {
        /// <summary>
        /// Calculates the stretch of centromere, in um
        /// </summary>
        public bool Tension { get; set; }

        public CentromereOptions(bool enabled)
        { Tension = enabled; }

        public static implicit operator CentromereOptions(bool value)
        { return new CentromereOptions(value); }

        public static implicit operator bool(CentromereOptions options)
        { return options.Tension; }

        public override string ToString()
        { return String.Format("{0}", Tension); }

        public override int GetHashCode()
        { return ToString().GetHashCode(); }
      }

      public class KmtOptions
      {
        /// <summary>
        /// Counts and stores table with the total number of KMT attachments, per kinetochore
        /// </summary>
        public bool Total { get; set; }

        /// <summary>
        /// The same as 'Total', but only for merotelic KMTs
        /// </summary>
        public bool Merotelic { get; set; }

        /// <summary>
        /// Classifies kinetochore pairs by their types in accordance with (Hauf 2004), in percents
        /// </summary>
        public bool ByTypes { get; set; }

        /// <summary>
        /// Applies (Cimini 2004)'s hypothesis to determine sister chromatids that are ready for anaphase, in percents
        /// </summary>
        public bool AnaphaseReady { get; set; }

        /// <summary>
        /// Compares our times (NEB -> anaphase) with reference RPE1 cells
        /// Source: https://doi.org/10.1038/s41467-018-04427-x
        /// </summary>
        public bool Dudka2018 { get; set; }

        /// <summary>
        /// Classifies the number of kinetochore-microtubule attachments for metaphase HeLa cells
        /// Compares these results with in silico data for 10-th (20-th, 30-th, etc) minute after NEB
        /// </summary>
        public bool McEwen2001 { get; set; }

        /// <summary>
        /// Compares our times (NEB -> anaphase) with reference HeLa cells
        /// Source: https://doi.org/10.1016/j.devcel.2004.06.006
        /// </summary>
        public bool Meraldi2004 { get; set; }

        /// <summary>
        /// Compares our times (NEB -> anaphase) with reference PtK1 cells
        /// Source: https://doi.org/10.1083/jcb.127.5.1301
        /// </summary>
        public bool Rieder1994 { get; set; }

        public KmtOptions(bool enabled)
        {
          Total = enabled;
          Merotelic = enabled;
          ByTypes = enabled;
          AnaphaseReady = enabled;
          Dudka2018 = enabled;
          McEwen2001 = enabled;
          Meraldi2004 = enabled;
          Rieder1994 = enabled;
        }

        public static implicit operator KmtOptions(bool value)
        { return new KmtOptions(value); }

        public static implicit operator bool(KmtOptions options)
        {
          return options.Total || options.Merotelic || options.ByTypes || options.AnaphaseReady ||
                 options.Meraldi2004 || options.Rieder1994;
        }

        public override string ToString()
        {
          StringBuilder sb = new StringBuilder();

          sb.Append("{");
          sb.Append(Total);
          sb.Append(",");
          sb.Append(Merotelic);
          sb.Append(",");
          sb.Append(ByTypes);
          sb.Append(",");
          sb.Append(AnaphaseReady);
          sb.Append(",");
          sb.Append(Dudka2018);
          sb.Append(",");
          sb.Append(McEwen2001);
          sb.Append(",");
          sb.Append(Meraldi2004);
          sb.Append(",");
          sb.Append(Rieder1994);
          sb.Append("}");

          return sb.ToString();
        }

        public override int GetHashCode()
        { return ToString().GetHashCode(); }
      }

      public class MovementOptions
      {
        /// <summary>
        /// Checks two rotation angles between sister chromatids and pole-to-pole segment, in degrees
        /// </summary>
        public bool Orientation { get; set; }

        /// <summary>
        /// Compares the orientation of in silico chromatid pairs with the ones from RPE1 cells
        /// </summary>
        public bool Magidson2011 { get; set; }

        public MovementOptions(bool enabled)
        {
          Orientation = enabled;
          Magidson2011 = enabled;
        }

        public static implicit operator MovementOptions(bool value)
        { return new MovementOptions(value); }

        public static implicit operator bool(MovementOptions value)
        { return value.Orientation; }

        public override String ToString()
        {
          StringBuilder sb = new StringBuilder();

          sb.Append("{");
          sb.Append(Orientation);
          sb.Append(",");
          sb.Append(Magidson2011);
          sb.Append("}");

          return sb.ToString();
        }

        public override int GetHashCode()
        { return ToString().GetHashCode(); }
      }

      /// <summary>
      /// Analyzes the state of centromeres
      /// </summary>
      public CentromereOptions Centromere { get; set; }

      /// <summary>
      /// Counts and classifies the number of kinetochore-microtubule attachments
      /// </summary>
      public KmtOptions KMTs { get; set; }

      /// <summary>
      /// Tracks chromosomes and analyzes their trajectories within cell
      /// </summary>
      public MovementOptions Movement { get; set; }

      public CellAnalyzers()
      {
        Centromere = false;
        KMTs = false;
        Movement = false;
      }

      public override string ToString()
      {
        StringBuilder sb = new StringBuilder();

        sb.Append("{");
        sb.Append(Centromere.ToString());
        sb.Append(",");
        sb.Append(KMTs.ToString());
        sb.Append(",");
        sb.Append(Movement.ToString());
        sb.Append("}");

        return sb.ToString();
      }

      public override int GetHashCode()
      { return ToString().GetHashCode(); }
    }
  }
}
