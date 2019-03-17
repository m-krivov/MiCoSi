using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Globalization;

namespace Mitosis
{
  public class PoleCoordinates : ICloneable
  {
    public struct Record
    {
      public double Time { get; private set; }
      public double LX { get; private set; }
      public double LY { get; private set; }
      public double LZ { get; private set; }
      public double RX { get; private set; }
      public double RY { get; private set; }
      public double RZ { get; private set; }

      internal Record(double time,
                      double lx, double ly, double lz,
                      double rx, double ry, double rz)
          : this()
      {
          Time = time;
          LX = lx; LY = ly; LZ = lz;
          RX = rx; RY = ry; RZ = rz;
      }
    };

    private List<Record> _records;
    public IEnumerable<Record> Records { get { return _records; } }

    public PoleCoordinates()
    { _records = new List<Record>(); }

    public PoleCoordinates(XmlNode root)
      : this()
    {
      String[] names = { "time", "lx", "ly", "lz", "rx", "ry", "rz" };
      double[] values = new double[7];
      foreach (var record in root.ChildNodes)
      {
        if (record is XmlElement)
        {
          var elem = record as XmlElement;
          for (int i = 0; i < names.Length; i++)
          {
            var attr = elem.GetAttribute(names[i]);
            double attrVal = 0.0;
            if (String.IsNullOrEmpty(attr) ||
                !Double.TryParse(attr, NumberStyles.Float, CultureInfo.InvariantCulture, out attrVal))
            { throw new ApplicationException(String.Format("Failed to get value for \"{0}\" attribute", names[i])); }
            values[i] = attrVal;
          }
        }

        _records.Add(new Record(values[0], values[1], values[2], values[3],
                                            values[4], values[5], values[6]));
      }
    }

    public PoleCoordinates(String xmlContent)
      : this(((Func<String, XmlNode>)(i =>
              {
                  XmlDocument doc = new XmlDocument();
                  doc.LoadXml(xmlContent);
                  var res = doc.SelectSingleNode("/Poles");
                  if (res == null)
                      throw new ApplicationException("Failed to located root section of XML");
                  return res;
              }
            ))(xmlContent))
    { /*nothing*/ }

    public void AddRecord(double time,
                          double lx, double ly, double lz,
                          double rx, double ry, double rz)
    {
      if (_records.Count != 0 && _records.Last().Time > time)
      { throw new ApplicationException("Records with pole coordinates must be sorted by time"); }

      _records.Add(new Record(time, lx, ly, lz, rx, ry, rz));
    }

    public void Export(XmlDocument doc, XmlNode root)
    {
      String[] names = { "time", "lx", "ly", "lz", "rx", "ry", "rz" };

      foreach (var rec in _records)
      {
        XmlElement sect = doc.CreateElement("Coords");
        root.AppendChild(sect);
        double[] vals = new double[] { rec.Time,
                                       rec.LX, rec.LY, rec.LZ,
                                       rec.RX, rec.RY, rec.RZ };

        for (int i = 0; i < vals.Length; i++)
        {
          XmlAttribute attr = doc.CreateAttribute(names[i]);
          attr.Value = vals[i].ToString(CultureInfo.InvariantCulture);
          sect.Attributes.Append(attr);
        }
      }
    }

    public override string ToString()
    {
      XmlDocument xml = new XmlDocument();
      xml.AppendChild(xml.CreateXmlDeclaration("1.0", "utf-8", "no"));
      XmlElement root = xml.CreateElement("Poles");
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
      var res = new PoleCoordinates();
      res._records = new List<Record>(_records);
      return res;
    }
  }
}
