using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  internal interface IFileNamingTemplate
  {
    String Generate(String extWithDot);
  }

  internal class DateTimeFileNamingTemplate : IFileNamingTemplate
  {
    private String _directory;
    private String _prefix;

    public DateTimeFileNamingTemplate(String directory, String prefix)
    {
      _directory = directory;
      _prefix = prefix;
    }

    public String Generate(String extWithDot)
    {
      StringBuilder goodNamePrefix = new StringBuilder();
      foreach (char letter in _prefix)
        if (Char.IsLetterOrDigit(letter))
          goodNamePrefix.Append(letter);
        else
          goodNamePrefix.Append('_');

      String res
        = System.IO.Path.Combine(_directory,
                                 goodNamePrefix.ToString() +
                                 " " +
                                 System.DateTime.Now.ToString().Replace(':', '.').Replace('/', '.').Replace('\\', '.'));

      int n = 0;
      String tmp = res + extWithDot;
      while (System.IO.File.Exists(tmp))
        tmp = res + " " + (++n) + extWithDot;
      res = tmp;

      try
      {
        System.IO.File.Create(res).Close();
        System.IO.File.Delete(res);
      }
      catch (Exception)
      {
        throw new ApplicationException("Cannot create file \"" + res + "\" - probably, there is no such directory or it is in RO mode");
      }

      return res;
    }
  }
}
