using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace MiCoSi
{
  internal class SettingsManagerBase
  {
    private Dictionary<String, SettingsObject> _objects;
    private String _fileName;

    /// <summary>
    /// Registeres object. Now it will be saved and stored automatically.
    /// Reference to the actual copy can be getted by provided name.
    /// </summary>
    protected void RegisterObject(String name, SettingsObject obj)
    {
      _objects.Add(name, obj);
    }

    /// <summary>
    /// Returns objects with required settings.
    /// </summary>
    protected SettingsObject GetObject(String name)
    {
      return _objects[name];
    }

    protected SettingsManagerBase(String fileName)
    {
      _objects = new Dictionary<string, SettingsObject>();
      _fileName = fileName;
    }

    /// <summary>
    /// Sets default values.
    /// </summary>
    public void Reset()
    {
      foreach (var so in _objects.Values)
        so.Reset();
    }

    /// <summary>
    /// Saves all registered objects to settings file.
    /// </summary>
    public void Save()
    {
      Stream s = null;
      try
      {
        s = File.Create(_fileName);
        BinaryFormatter sf = new BinaryFormatter();
        sf.Serialize(s, _objects.Count);
        foreach (var so in _objects)
        {
          sf.Serialize(s, new KeyValuePair<String, SettingsObject>(so.Key, so.Value));
        }
        s.Close();
      }
      catch (Exception)
      {
        if (s != null)
          s.Close();
        throw;
      }
    }

    /// <summary>
    /// Loads strored settings into current instance.
    /// </summary>
    public bool Load()
    {
      Stream s = null;
      try
      {
        if (File.Exists(_fileName))
        {
          s = File.OpenRead(_fileName);
          BinaryFormatter sf = new BinaryFormatter();
          int objectCount = (int)sf.Deserialize(s);
          for (int i = 0; i < objectCount; i++)
          {
            var record = (KeyValuePair<String, SettingsObject>)sf.Deserialize(s);
            _objects[record.Key] = record.Value;
          }
          s.Close();
          return true;
        }
        else
        {
          Reset();
          return false;
        }
      }
      catch (Exception)
      {
        if (s != null)
          s.Close();
        throw;
      }
    }

    /// <summary>
    /// Deletes file with stored settings (if such file exosts).
    /// </summary>
    public void DeleteSettingsFile()
    {
      try
      {
        if (File.Exists(_fileName))
          File.Delete(_fileName);
      }
      catch (Exception) { }
    }

    /// <summary>
    /// Stores all settings values into state object and returns it.
    /// </summary>
    public Object ExportState()
    {
      var res = new Dictionary<String, SettingsObject>();

      foreach (var so in _objects)
      {
        res.Add(so.Key, (SettingsObject)so.Value.Clone());
      }

      return res;
    }

    /// <summary>
    /// Applies previously stored settings from state object.
    /// </summary>
    public void ImportState(Object state)
    {
      if (!(state is Dictionary<String, SettingsObject>))
        throw new ApplicationException("Internal error at ImportState(): \"state\" is not a state object");

      var settings = state as Dictionary<String, SettingsObject>;

      foreach (var so in settings)
      {
        _objects[so.Key] = (SettingsObject)so.Value.Clone();
      }
    }
  }
}
