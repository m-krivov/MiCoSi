using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis.DxRender
{
  internal class StyleCollection : ICloneable
  {
    public List<StyleObject> AllStyles { get; private set; }

    public PoleStyle Poles { get; private set; }
    public TubeStyle Tubes { get; private set; }
    public ChromosomeStyle Chromosomes { get; private set; }
    public SpringStyle Springs { get; private set; }
    public CellStyle Cell { get; private set; }

    public StyleCollection()
    {
      List<StyleObject> _allStyles = new List<StyleObject>();
      AllStyles = _allStyles;

      Poles = new PoleStyle();
      _allStyles.Add(Poles);

      Tubes = new TubeStyle();
      _allStyles.Add(Tubes);

      Chromosomes = new ChromosomeStyle();
      _allStyles.Add(Chromosomes);

      Springs = new SpringStyle();
      _allStyles.Add(Springs);

      Cell = new CellStyle();
      _allStyles.Add(Cell);
    }

    public void SetDefault()
    {
      foreach (var style in AllStyles)
        style.SetDefault();
    }

    public Object Clone()
    {
      StyleCollection res = new StyleCollection();

      List<StyleObject> allStyles = new List<StyleObject>();
      res.AllStyles = allStyles;

      res.Poles = Poles.Clone() as PoleStyle;
      allStyles.Add(res.Poles);

      res.Tubes = Tubes.Clone() as TubeStyle;
      allStyles.Add(res.Tubes);

      res.Chromosomes = Chromosomes.Clone() as ChromosomeStyle;
      allStyles.Add(res.Chromosomes);

      res.Springs = Springs.Clone() as SpringStyle;
      allStyles.Add(res.Springs);

      res.Cell = Cell.Clone() as CellStyle;
      allStyles.Add(res.Cell);

      return res;
    }
  }
}
