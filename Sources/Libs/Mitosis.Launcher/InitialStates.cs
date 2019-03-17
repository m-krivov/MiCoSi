using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Globalization;

namespace Mitosis
{
  public class InitialStates : ICloneable
  {
    public class MtRecord
    {
      public PoleType Pole { get; private set; }
      public double DirectionX { get; private set; }
      public double DirectionY { get; private set; }
      public double DirectionZ { get; private set; }
      public double Length { get; private set; }
      public MTState State { get; private set; }

      internal MtRecord(PoleType pole,
                        double direction_x,
                        double direction_y,
                        double direction_z,
                        double length,
                        MTState state)
      {
        Pole = pole;
        DirectionX = direction_x;
        DirectionY = direction_y;
        DirectionZ = direction_z;
        Length = length;
        State = state;
      }

      internal List<Tuple<String, String>> GetAttributes()
      {
        var res = new List<Tuple<String, String>>();
        res.Add(new Tuple<String, String>("pole", Pole == PoleType.Left ? "left" : "right"));
        res.Add(new Tuple<String, String>("direction_x", DirectionX.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("direction_y", DirectionY.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("direction_z", DirectionZ.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("length", Length.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("state", State == MTState.Polymerization
                                                            ? "polymerization"
                                                            : "depolymerization"));
        return res;
      }
    };

    public class ChromosomeRecord
    {
      public double PositionX { get; private set; }
      public double PositionY { get; private set; }
      public double PositionZ { get; private set; }
      public double OrientationX { get; private set; }
      public double OrientationY { get; private set; }
      public double OrientationZ { get; private set; }

      internal ChromosomeRecord(double position_x,
                                double position_y,
                                double position_z,
                                double orientation_x,
                                double orientation_y,
                                double orientation_z)
      {
        PositionX = position_x;
        PositionY = position_y;
        PositionZ = position_z;
        OrientationX = orientation_x;
        OrientationY = orientation_y;
        OrientationZ = orientation_z;
      }

      internal List<Tuple<String, String>> GetAttributes()
      {
        var res = new List<Tuple<String, String>>();
        res.Add(new Tuple<String, String>("position_x", PositionX.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("position_y", PositionY.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("position_z", PositionZ.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("orientation_x", OrientationX.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("orientation_y", OrientationY.ToString(CultureInfo.InvariantCulture)));
        res.Add(new Tuple<String, String>("orientation_z", OrientationZ.ToString(CultureInfo.InvariantCulture)));
        return res;
      }
    }

    private List<MtRecord> _MTs;
    private List<ChromosomeRecord> _chromosomes;

    public IEnumerable<MtRecord> MTs { get { return _MTs; } }
    public IEnumerable<ChromosomeRecord> Chromosomes { get { return _chromosomes; } }

    public InitialStates()
    {
      _MTs = new List<MtRecord>();
      _chromosomes = new List<ChromosomeRecord>();
    }

    public InitialStates(XmlNode root)
      : this()
    {
      var mts = root.SelectSingleNode("MTs");
      if (mts == null) throw new ApplicationException("Failed to locate section with MTs");
      String[] mts_names = new String[] { "pole", "direction_x", "direction_y", "direction_z", "length", "state" };
      String[] mts_values = new String[6];
      foreach (var mt in mts.ChildNodes)
      {
        if (mt is XmlElement)
        {
          var elem = mt as XmlElement;
          for (int i = 0; i < mts_names.Length; i++)
          {
            var attr = elem.GetAttribute(mts_names[i]);
            if (String.IsNullOrEmpty(attr))
                throw new ApplicationException(String.Format("Failed to get value for \"{0}\" attribute", mts_names[i]));
            mts_values[i] = attr;
          }

          double direction_x, direction_y, direction_z, length;
          if (!(mts_values[0].ToLower() == "left" || mts_values[0].ToLower() == "right") ||
              !Double.TryParse(mts_values[1], NumberStyles.Float, CultureInfo.InvariantCulture, out direction_x) ||
              !Double.TryParse(mts_values[2], NumberStyles.Float, CultureInfo.InvariantCulture, out direction_y) ||
              !Double.TryParse(mts_values[3], NumberStyles.Float, CultureInfo.InvariantCulture, out direction_z) ||
              !Double.TryParse(mts_values[4], NumberStyles.Float, CultureInfo.InvariantCulture, out length) ||
              !(mts_values[5].ToLower() == "polymerization" || mts_values[5].ToLower() == "depolymerization"))
              throw new ApplicationException("Failed to parse some attributes");
          AddMT(mts_values[0].ToLower() == "left" ? PoleType.Left : PoleType.Right,
                direction_x, direction_y, direction_z, length,
                mts_values[5].ToLower() == "polymerization" ? MTState.Polymerization : MTState.Depolymerization);
        }
      }

      var chrs = root.SelectSingleNode("Chromosomes");
      if (chrs == null)
      {
        throw new ApplicationException("Failed to locate section with Chromosomoes");
      }
      var chr_names = new String[] { "position_x", "position_y", "position_z",
                                     "orientation_x", "orientation_y", "orientation_z" };
      var chr_values = new double[6];
      foreach (var chr in chrs)
      {
        if (chr is XmlElement)
        {
          var elem = chr as XmlElement;
          for (int i = 0; i < chr_names.Length; i++)
          {
            var attr = elem.GetAttribute(chr_names[i]);
            double attr_value = 0.0;
            if (String.IsNullOrEmpty(attr) ||
                !Double.TryParse(attr, NumberStyles.Float, CultureInfo.InvariantCulture, out attr_value))
            { throw new ApplicationException(String.Format("Failed to get value for \"{0}\" attribute", chr_names[i])); }
            chr_values[i] = attr_value;
          }
          AddChromosomePair(chr_values[0], chr_values[1], chr_values[2],
                            chr_values[3], chr_values[4], chr_values[5]);
        }
      }
    }

    public InitialStates(String xmlContent)
      : this(((Func<String, XmlNode>)(i =>
                {
                    XmlDocument doc = new XmlDocument();
                    doc.LoadXml(xmlContent);
                    var res = doc.SelectSingleNode("/InitialStates");
                    if (res == null)
                        throw new ApplicationException("Failed to located root section of XML");
                    return res;
                }
             ))(xmlContent))
    { /*nothing*/ }

    public void AddMT(PoleType pole,
                      double direction_x,
                      double direction_y,
                      double direction_z,
                      double length,
                      MTState state)
    { _MTs.Add(new MtRecord(pole, direction_x, direction_y, direction_z, length, state)); }

    public void AddChromosomePair(double position_x,
                                  double position_y,
                                  double position_z,
                                  double orientation_x,
                                  double orientation_y,
                                  double orientation_z)
    { _chromosomes.Add(new ChromosomeRecord(position_x, position_y, position_z, orientation_x, orientation_y, orientation_z)); }

    public void Export(XmlDocument doc, XmlNode root)
    {
      String[] names = new String[] { "MTs", "Chromosomes" };
      String[] subNames = new String[] { "MT", "ChromosomePair" };
      var records = new List<List<Tuple<String, String>>>[2];
      records[0] = new List<List<Tuple<String, String>>>();
      foreach (var mt in _MTs)
      { records[0].Add(mt.GetAttributes()); }
      records[1] = new List<List<Tuple<String, String>>>();
      foreach (var chr in _chromosomes)
      { records[1].Add(chr.GetAttributes()); }

      for (int i = 0; i < names.Length; i++)
      {
        XmlElement section = doc.CreateElement(names[i]);
        root.AppendChild(section);
        foreach (var rec in records[i])
        {
          XmlElement subSection = doc.CreateElement(subNames[i]);
          section.AppendChild(subSection);
          foreach (var attribute in rec)
          {
            XmlAttribute attr = doc.CreateAttribute(attribute.Item1);
            attr.Value = attribute.Item2;
            subSection.Attributes.Append(attr);
          }
        }
      }
    }

    public override string ToString()
    {
      XmlDocument xml = new XmlDocument();
      xml.AppendChild(xml.CreateXmlDeclaration("1.0", "utf-8", "no"));
      XmlElement root = xml.CreateElement("InitialStates");
      xml.AppendChild(root);
      Export(xml, root);
      using (var stringWriter = new StringWriter())
      using (var xmlWriter = XmlWriter.Create(stringWriter))
      {
        xml.WriteTo(xmlWriter);
        xmlWriter.Flush();
        return stringWriter.GetStringBuilder().ToString();
      }
    }

    public object Clone()
    {
      var res = new InitialStates();
      res._MTs = new List<MtRecord>(_MTs);
      res._chromosomes = new List<ChromosomeRecord>(_chromosomes);
      return res;
    }
  }
}
