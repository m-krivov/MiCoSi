﻿
using System;

namespace MiCoSi.DxRender
{
  internal interface IStyleAspect
  {
    String TechniqueName { get; }

    T Resolve<T>(IObjectWithID obj)
      where T : StyleObject;
  }
}
