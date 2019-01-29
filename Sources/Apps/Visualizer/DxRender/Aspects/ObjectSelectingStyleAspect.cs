using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis.DxRender
{
  class ObjectSelectingStyleAspect : IStyleAspect
  {
    private delegate IObjectWithID ObjectRetriever(Cell cell, int id);

    private static IObjectWithID RetrievePole(Cell cell, int id)
    {
      if (cell.GetPole(PoleType.Left).ID == id)
        return cell.GetPole(PoleType.Left);
      else if (cell.GetPole(PoleType.Right).ID == id)
        return cell.GetPole(PoleType.Right);
      else
        throw new ApplicationException("Internal error - there is no pole with such ID");
    }

    private static IObjectWithID RetrieveMT(Cell cell, int id)
    {
      if (cell.MTs is List<MT>)
      {
        List<MT> mts = cell.MTs as List<MT>;
        if (id >= mts.Count)
          throw new ApplicationException("Internal error - there is no MT with such ID");
        return mts[id];
      }

      throw new ApplicationException("Internal error - not implemented yet");
    }

    private static IObjectWithID RetrieveChromosome(Cell cell, int id)
    {
      if (cell.Chromosomes is List<Chromosome>)
      {
        List<Chromosome> chrs = cell.Chromosomes as List<Chromosome>;
        if (id >= chrs.Count)
          throw new ApplicationException("Internal error - there is no chromosome with such ID");
        return chrs[id];
      }

      throw new ApplicationException("Internal error - not implemented yet");
    }

    private readonly StyleCollection _styles;
    private readonly int _bitsPerMajorID;
    private readonly int _bitsPerMinorID;
    private readonly Dictionary<Type, int> _typeToMajorId;
    private readonly Dictionary<int, ObjectRetriever> _majorIdToRetriever;

    public ObjectSelectingStyleAspect()
    {
      _styles = StyleManager.CreateByName(StyleManager.DefaultName);

      _bitsPerMajorID = 4;
      _bitsPerMinorID = 20;

      _typeToMajorId = new Dictionary<Type, int>();
      _typeToMajorId.Add(typeof(Pole), 1);
      _typeToMajorId.Add(typeof(MT), 2);
      _typeToMajorId.Add(typeof(Chromosome), 4);

      _majorIdToRetriever = new Dictionary<int, ObjectRetriever>();
      _majorIdToRetriever.Add(1, RetrievePole);
      _majorIdToRetriever.Add(2, RetrieveMT);
      _majorIdToRetriever.Add(4, RetrieveChromosome);
    }

    public string TechniqueName
    {
      get { return "Solid"; }
    }

    public SlimDX.Color4 EncodeColor(IObjectWithID obj)
    {
      int major = 0;
      if (!_typeToMajorId.TryGetValue(obj.GetType(), out major))
        return new SlimDX.Color4(0x00000000);

      int minor = (int)obj.ID;

      if (major >= (1 << _bitsPerMajorID) || minor >= (1 << _bitsPerMinorID))
        throw new ApplicationException("Internal error - too much selectable objects");

      return new SlimDX.Color4(0x00000000 | (major << _bitsPerMinorID) | minor);
    }

    public IObjectWithID DecodeColor(Cell cell, SlimDX.Color4 color)
    {
      int colorValue = ((int)(color.Red * 255) << 16) |
                        ((int)(color.Green * 255) << 8) |
                        (int)(color.Blue * 255);
      
      int major = (colorValue >> _bitsPerMinorID) & ~(~0 << _bitsPerMajorID);
      ObjectRetriever retriever;
      if (!_majorIdToRetriever.TryGetValue(major, out retriever))
        return null;

      int minor = colorValue & ~(~0 << _bitsPerMinorID);
      return retriever(cell, minor);
    }

    public T Resolve<T>(IObjectWithID obj) where T : StyleObject
    {
      T res = (_styles.AllStyles.Find(styleObject => styleObject.GetType() == typeof(T)) as T).Clone() as T;
      
      SlimDX.Vector4 color = EncodeColor(obj).ToVector4();
      Material mat = new Material(color, color, color);
      res.SetProperties(mat);

      return res;
    }
  }
}
