
using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;

namespace Mitosis.DxRender
{
  internal abstract class StyleObject : ICloneable
  {
    internal class WrappedValue<T>
    {
      public WrappedValue()
      { Value = default(T); }

      public T Value { get; set; }
    }

    private readonly Dictionary<String, WrappedValue<Material> > _materials;
    private readonly Dictionary<String, WrappedValue<double> > _doubles;

    protected StyleObject()
    {
      _materials = new Dictionary<String, WrappedValue<Material> >();
      _doubles = new Dictionary<String, WrappedValue<double> >();
    }

    protected void RegisterObject(String symName, WrappedValue<Material> obj)
    { _materials[symName] = obj; }

    protected void RegisterObject(String symName, WrappedValue<double> obj)
    { _doubles[symName] = obj; }

    private static void SetProperty_Generic<T>(Dictionary<String, WrappedValue<T>> collection, String symName, T value)
    {
      WrappedValue<T> val = null;
      if (!collection.TryGetValue(symName, out val))
        throw new ApplicationException("No such property");
      val.Value = value;
    }

    public void SetProperty(String symName, Material value)
    { SetProperty_Generic<Material>(_materials, symName, value); }

    public void SetProperty(String symName, double value)
    { SetProperty_Generic<double>(_doubles, symName, value); }

    private static void SetProperties_Generic<T>(Dictionary<String, WrappedValue<T>> collection, T value)
    {
      foreach (var rec in collection)
        rec.Value.Value = value;    // Achtung.Achtung = achtung!
    }

    public void SetProperties(Material value)
    { SetProperties_Generic<Material>(_materials, value); }

    public void SetProperties(double value)
    { SetProperties_Generic<double>(_doubles, value); }

    public abstract void SetDefault();

    public abstract String Name { get; }

    public abstract IEnumerable<KeyValuePair<String, StylePropertyType>> Properties { get; }

    public abstract Object Clone();
  }

  internal class PoleStyle : StyleObject,  IPoleStyle
  {
    private readonly WrappedValue<double> _radius = new WrappedValue<double>();
    private readonly WrappedValue<Material> _leftPole = new WrappedValue<Material>();
    private readonly WrappedValue<Material> _rightPole = new WrappedValue<Material>();

    public PoleStyle()
    {
      RegisterObject("Radius", _radius);
      RegisterObject("LeftPoleMaterial", _leftPole);
      RegisterObject("RightPoleMaterial", _rightPole);

      _properties = new KeyValuePair<String, StylePropertyType>[]
        {
          new KeyValuePair<String, StylePropertyType>("Radius", StylePropertyType.propDouble),
          new KeyValuePair<String, StylePropertyType>("LeftPoleMaterial", StylePropertyType.propMaterial),
          new KeyValuePair<String, StylePropertyType>("RightPoleMaterial", StylePropertyType.propMaterial)
        };

      SetDefault();
    }

    public override String Name
    {
      get { return "PoleStyle"; }
    }

    private readonly IEnumerable<KeyValuePair<String, StylePropertyType>> _properties;
    public override IEnumerable<KeyValuePair<String, StylePropertyType>> Properties
    {
      get { return _properties; }
    }

    public double Radius
    {
      get { return _radius.Value; }
      private set { _radius.Value = value; }
    }

    public Material LeftPole
    {
      get { return _leftPole.Value; }
      private set { _leftPole.Value = value; }
    }

    public Material RightPole
    {
      get { return _rightPole.Value; }
      private set { _rightPole.Value = value; }
    }

    public override void SetDefault()
    {
      Radius = 0.01;
      LeftPole = new Material(new Vector4(10.0f / 255, 230.0f / 255, 10.0f / 255, 1.0f),
                              new Vector4(0.0f, 1.0f, 0.0f, 1.0f),
                              new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      RightPole = new Material(new Vector4(10.0f / 255, 230.0f / 255, 10.0f / 255, 1.0f),
                               new Vector4(0.0f, 1.0f, 0.0f, 1.0f),
                               new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    public override Object Clone()
    {
      PoleStyle res = new PoleStyle();
      res.Radius = Radius;
      res.LeftPole = LeftPole;
      res.RightPole = RightPole;
      return res;
    }
  }

  internal class TubeStyle : StyleObject, ITubeStyle
  {
    private readonly WrappedValue<double> _width = new WrappedValue<double>();
    private readonly WrappedValue<Material> _freeLeftPoleTube = new WrappedValue<Material>();
    private readonly WrappedValue<Material> _boundLeftPoleTube = new WrappedValue<Material>();
    private readonly WrappedValue<Material> _freeRightPoleTube = new WrappedValue<Material>();
    private readonly WrappedValue<Material> _boundRightPoleTube = new WrappedValue<Material>();

    public TubeStyle()
    {
      RegisterObject("Width", _width);
      RegisterObject("FreeLeftPoleTube", _freeLeftPoleTube);
      RegisterObject("BoundLeftPoleTube", _boundLeftPoleTube);
      RegisterObject("FreeRightPoleTube", _freeRightPoleTube);
      RegisterObject("BoundRightPoleTube", _boundRightPoleTube);

      _properties = new KeyValuePair<String, StylePropertyType>[]
        {
          new KeyValuePair<String, StylePropertyType>("Width", StylePropertyType.propDouble),
          new KeyValuePair<String, StylePropertyType>("FreeLeftPoleTube", StylePropertyType.propMaterial),
          new KeyValuePair<String, StylePropertyType>("BoundLeftPoleTube", StylePropertyType.propMaterial),
          new KeyValuePair<String, StylePropertyType>("FreeRightPoleTube", StylePropertyType.propMaterial),
          new KeyValuePair<String, StylePropertyType>("BoundRightPoleTube", StylePropertyType.propMaterial)
        };

      SetDefault();
    }

    public override String Name
    {
      get { return "TubeStyle"; }
    }

    private readonly IEnumerable<KeyValuePair<String, StylePropertyType>> _properties;
    public override IEnumerable<KeyValuePair<String, StylePropertyType>> Properties
    {
      get { return _properties; }
    }

    public double Width
    {
      get { return _width.Value; }
      private set { _width.Value = value; }
    }

    public Material FreeLeftPoleTube
    {
      get { return _freeLeftPoleTube.Value; }
      private set { _freeLeftPoleTube.Value = value; }
    }

    public Material BoundLeftPoleTube
    {
      get { return _boundLeftPoleTube.Value; }
      private set { _boundLeftPoleTube.Value = value; }
    }

    public Material FreeRightPoleTube
    {
      get { return _freeRightPoleTube.Value; }
      private set { _freeRightPoleTube.Value = value; }
    }

    public Material BoundRightPoleTube
    {
      get { return _boundRightPoleTube.Value; }
      private set { _boundRightPoleTube.Value = value; }
    }

    public override void SetDefault()
    {
      Width = 0.002;
      FreeLeftPoleTube = new Material(new Vector4(100.0f / 255, 100.0f / 255, 50.0f / 255, 1.0f),
                                      new Vector4(100.0f / 255, 100.0f / 255, 100.0f / 255, 1.0f),
                                      new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      BoundLeftPoleTube = new Material(new Vector4(200.0f / 255, 200.0f / 255, 50.0f / 255, 1.0f),
                                       new Vector4(200.0f / 255, 200.0f / 255, 100.0f / 255, 1.0f),
                                       new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      FreeRightPoleTube = new Material(new Vector4(50.0f / 255, 100.0f / 255, 100.0f / 255, 1.0f),
                                       new Vector4(100.0f / 255, 100.0f / 255, 100.0f / 255, 1.0f),
                                       new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      BoundRightPoleTube = new Material(new Vector4(50.0f / 255, 200.0f / 255, 200.0f / 255, 1.0f),
                                        new Vector4(100.0f / 255, 200.0f / 255, 200.0f / 255, 1.0f),
                                        new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    public override Object Clone()
    {
      TubeStyle res = new TubeStyle();
      
      res.Width = Width;
      res.FreeLeftPoleTube = FreeLeftPoleTube;
      res.BoundLeftPoleTube = BoundLeftPoleTube;
      res.FreeRightPoleTube = FreeRightPoleTube;
      res.BoundRightPoleTube = BoundRightPoleTube;

      return res;
    }
  }

  internal class ChromosomeStyle : StyleObject, IChromosomeStyle
  {
    private readonly WrappedValue<Material> _kinetohore = new WrappedValue<Material>();
    private readonly WrappedValue<Material> _hand = new WrappedValue<Material>();

    public ChromosomeStyle()
    {
      RegisterObject("Hand", _hand);
      RegisterObject("Kinetohore", _kinetohore);

      _properties = new KeyValuePair<String, StylePropertyType>[]
        {
          new KeyValuePair<String, StylePropertyType>("Hand", StylePropertyType.propMaterial),
          new KeyValuePair<String, StylePropertyType>("Kinetohore", StylePropertyType.propMaterial)
        };

      SetDefault();
    }

    public override String Name
    {
      get { return "ChromosomeStyle"; }
    }

    private readonly IEnumerable<KeyValuePair<String, StylePropertyType>> _properties;
    public override IEnumerable<KeyValuePair<String, StylePropertyType>> Properties
    {
      get { return _properties; }
    }

    public Material Hand
    {
      get { return _hand.Value; }
      private set { _hand.Value = value; }
    }

    public Material Kinetohore
    {
      get { return _kinetohore.Value; }
      private set { _kinetohore.Value = value; }
    }

    public override void SetDefault()
    {
      Hand = new Material(new Vector4(50.0f / 255, 200.0f / 255, 200.0f / 255, 1.0f),
                          new Vector4(20.0f / 255, 250.0f / 255, 250.0f / 255, 1.0f),
                          new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      Kinetohore = new Material(new Vector4(50.0f / 255, 200.0f / 255, 200.0f / 255, 1.0f),
                                new Vector4(20.0f / 255, 250.0f / 255, 250.0f / 255, 1.0f),
                                new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    public override Object Clone()
    {
      ChromosomeStyle res = new ChromosomeStyle();

      res.Hand = Hand;
      res.Kinetohore = Kinetohore;

      return res;
    }
  }

  internal class SpringStyle : StyleObject, ISpringStyle
  {
    private readonly WrappedValue<double> _width = new WrappedValue<double>();
    private readonly WrappedValue<Material> _spring = new WrappedValue<Material>();

    public SpringStyle()
    {
      RegisterObject("Width", _width);
      RegisterObject("Spring", _spring);

      _properties = new KeyValuePair<String, StylePropertyType>[]
        {
          new KeyValuePair<String, StylePropertyType>("Width", StylePropertyType.propDouble),
          new KeyValuePair<String, StylePropertyType>("Spring", StylePropertyType.propMaterial)
        };

      SetDefault();
    }

    public override String Name
    {
      get { return "SpringStyle"; }
    }

    private readonly IEnumerable<KeyValuePair<String, StylePropertyType>> _properties;
    public override IEnumerable<KeyValuePair<String, StylePropertyType>> Properties
    {
      get { return _properties; }
    }

    public double Width
    {
      get { return _width.Value; }
      private set { _width.Value = value; }
    }

    public Material Spring
    {
      get { return _spring.Value; }
      private set { _spring.Value = value; }
    }

    public override void SetDefault()
    {
      Width = 0.01;
      Spring = new Material(new Vector4(50.0f / 255, 200.0f / 255, 200.0f / 255, 1.0f),
                            new Vector4(20.0f / 255, 250.0f / 255, 250.0f / 255, 1.0f),
                            new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    public override Object Clone()
    {
      SpringStyle res = new SpringStyle();

      res.Width = Width;
      res.Spring = Spring;

      return res;
    }
  }

  internal class CellStyle : StyleObject, ICellStyle
  {
    private readonly WrappedValue<Material> _cell = new WrappedValue<Material>();

    public CellStyle()
    {
      RegisterObject("Cell", _cell);

      _properties = new KeyValuePair<String, StylePropertyType>[]
        {
          new KeyValuePair<String, StylePropertyType>("Cell", StylePropertyType.propMaterial),
        };

      SetDefault();
    }

    public override String Name
    {
      get { return "CellStyle"; }
    }

    private readonly IEnumerable<KeyValuePair<String, StylePropertyType>> _properties;
    public override IEnumerable<KeyValuePair<String, StylePropertyType>> Properties
    {
      get { return _properties; }
    }

    public Material Cell
    {
      get { return _cell.Value; }
      private set { _cell.Value = value; }
    }

    public override void SetDefault()
    {
      Cell = new Material(new Vector4(0.15f, 0.3f, 0.15f, 1.0f),
                          new Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                          new Vector4(0.1f, 0.5f, 0.1f, 1.0f));
    }

    public override Object Clone()
    {
      CellStyle res = new CellStyle();

      res.Cell = Cell;

      return res;
    }
  }
}
