using System;
using System.Collections.Generic;
using System.Text;

namespace Mitosis.DxRender
{
  internal enum StylePropertyType
  {
    propDouble,
    propMaterial
  }

  internal interface IPoleStyle
  {
    Material RightPole { get; }
    Material LeftPole { get; }
    double Radius { get; }
  }

  internal interface ITubeStyle
  {
    double Width { get; }
    Material FreeLeftPoleTube { get; }
    Material BoundLeftPoleTube { get; }
    Material FreeRightPoleTube { get; }
    Material BoundRightPoleTube { get; }
  }

  internal interface IChromosomeStyle
  {
    Material Hand { get; }
    Material Kinetohore { get; }
  }

  internal interface ISpringStyle
  {
    double Width { get; }
    Material Spring { get; }
  }

  internal interface ICellStyle
  {
    Material Cell { get; }
  }
}
