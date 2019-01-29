using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis
{
  internal interface IOnTickActionContext : IDisposable { }

  internal interface IOnTickAction : IDisposable
  {
    void Perform();

    IOnTickActionContext DisposeAndExportContext();
  }
}
