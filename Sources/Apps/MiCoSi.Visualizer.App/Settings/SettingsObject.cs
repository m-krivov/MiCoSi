using System;
using System.Collections.Generic;
using System.Text;

namespace MiCoSi
{
  [Serializable()]
  internal abstract class SettingsObject
  {
    public abstract void Reset();

    public abstract Object Clone();
  }
}
