using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace MiCoSi.DxRender
{
  internal class StyleManager
  {
    public static String DefaultName
    {
      get { return "Default"; }
    }

    private static float ParseSingle(String sval)
    {
      return Single.Parse(sval,
                          System.Globalization.NumberStyles.Float,
                          System.Globalization.NumberFormatInfo.InvariantInfo);
    }

    private static double LoadDoubleParameter(XmlNode xml)
    {
      foreach (XmlAttribute attr in xml.Attributes)
      {
        if (attr.Name == "Value")
        { return ParseSingle(attr.Value); }
      }
      
      throw new ApplicationException("Internal error - corrupted style file");
    }

    private static SlimDX.Vector4 ParseVector(String str)
    {
      String[] comps = str.Split(new String[] { " ", "(", ")", "," },
                                 StringSplitOptions.RemoveEmptyEntries);
      if (comps.Length != 3)
      { throw new ApplicationException("Failed to parse vector"); }

      float x = ParseSingle(comps[0]);
      float y = ParseSingle(comps[1]);
      float z = ParseSingle(comps[2]);

      return new SlimDX.Vector4(x, y, z, 1.0f);
    }

    private static Material LoadMaterialParameter(XmlNode xml)
    {
      SlimDX.Vector4 diffColor = new SlimDX.Vector4();
      SlimDX.Vector4 specColor = new SlimDX.Vector4();
      SlimDX.Vector4 rimColor = new SlimDX.Vector4();

      Dictionary<String, bool> loadedFields = new Dictionary<string, bool>();
      loadedFields["DiffColor"] = false;
      loadedFields["SpecColor"] = false;
      loadedFields["RimColor"] = false;

      foreach (XmlAttribute attr in xml.Attributes)
      {
        if (attr.Name == "DiffColor")
        {
          diffColor = ParseVector(attr.Value);
          loadedFields["DiffColor"] = true;
        }
        else if (attr.Name == "SpecColor")
        {
          specColor = ParseVector(attr.Value);
          loadedFields["SpecColor"] = true;
        }
        else if (attr.Name == "RimColor")
        {
          rimColor = ParseVector(attr.Value);
          loadedFields["RimColor"] = true;
        }
      }

      if (!(loadedFields["DiffColor"] && loadedFields["SpecColor"] && loadedFields["RimColor"]))
      { throw new ApplicationException("Internal error - failed to load some section"); }

      return new Material(diffColor, specColor, rimColor);
    }
    
    private static void LoadStyle(StyleObject obj, XmlNode xml)
    {
      Dictionary<String, bool> loadedFields = new Dictionary<string, bool>();

      foreach (var val in obj.Properties)
      {
        loadedFields[val.Key] = false;
      }

      var allProps = new List<KeyValuePair<String, StylePropertyType>>(obj.Properties);
      foreach (XmlNode subEl in xml.ChildNodes)
      {
        var found = allProps.Find(rec => rec.Key == subEl.Name);

        if (found.Value == StylePropertyType.propMaterial)
        {
          obj.SetProperty(found.Key, LoadMaterialParameter(subEl));
        }
        else if (found.Value == StylePropertyType.propDouble)
        { obj.SetProperty(found.Key, LoadDoubleParameter(subEl)); }
        else
        { throw new ApplicationException("Internal error - unknown section in style file"); }

        loadedFields[found.Key] = true;
      }

      foreach (var val in loadedFields.Values)
      {
        if (!val)
        {
          throw new ApplicationException("Internal error - failed to load some section");
        }
      }
    }

    private static void LoadStyleCollection(StyleCollection collection, String xmlFilename)
    {
      XmlDocument doc = new XmlDocument();
      doc.Load(xmlFilename);

      XmlNode root = null;

      foreach (XmlNode node in doc.ChildNodes)
      {
        if (node.Name == "Style")
        {
          root = node;
          break;
        }
      }

      Dictionary<String, bool> loadedStyles = new Dictionary<string, bool>();

      foreach (var obj in collection.AllStyles)
      {
        loadedStyles[obj.Name] = false;
      }

      var allStyles = new List<StyleObject>(collection.AllStyles);
      foreach (XmlNode node in root.ChildNodes)
      {
        StyleObject match = allStyles.Find(styleObject => styleObject.Name.ToLower() == node.Name.ToLower());

        if (match != null)
        {
          LoadStyle(match, node);
          loadedStyles[match.Name] = true;
        }
      }

      foreach (StyleObject obj in collection.AllStyles)
      {
        if (!loadedStyles[obj.Name])
        {
          throw new ApplicationException("Failed to load some sections from style file");
        }
      }
    }

    public static StyleCollection CreateByName(String styleName)
    {
      StyleCollection res = new StyleCollection();
      String styleDir = Directories.GetConfigDirectory();
      String styleFile = System.IO.Path.Combine(styleDir, styleName);

      if (styleName.ToLower() == DefaultName.ToLower())
      {
        res.SetDefault();
        return res;
      }
      else if (System.IO.File.Exists(styleFile))
      {
        try
        {
          LoadStyleCollection(res, styleFile);
        }
        catch (Exception)
        {
          UserErrors.ConfigFileCorrupted(styleName, "using default style");
          res.SetDefault();
        }

        return res;
      }
      else
      {
        throw new ApplicationException("Internal error - cannot find style file");
      }
    }

    public static List<String> GetKnownNames()
    {
      String styleDir = Directories.GetConfigDirectory();
      String[] styles = System.IO.Directory.GetFiles(styleDir);

      List<String> res = new List<string>();
      bool hasDefault = false;

      foreach (var style in styles)
      {
        System.IO.FileInfo fi = new System.IO.FileInfo(style);

        if (fi.Extension.ToLower() == ".xml")
        {
          res.Add(fi.Name);

          if (fi.Name.ToLower() == DefaultName.ToLower())
          {
            hasDefault = true;
          }
        }
      }

      if (!hasDefault)
      {
        res.Insert(0, DefaultName);
      }

      return res;
    }
  }
}
