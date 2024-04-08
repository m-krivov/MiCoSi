using System;
using System.Collections.Generic;
using System.Text;

namespace MiCoSi
{
  internal class MouseState
  {
    public float DX { get; private set; }
    public float DY { get; private set; }
    public float DZ { get; private set; }

    public bool LeftButtonPressed { get; private set; }
    public bool RightButtonPressed { get; private set; }

    public MouseState(float dx, float dy, float dz,
                      bool isLeftButtonPressed, bool isRightButtonPressed)
    {
      DX = dx;
      DY = dy;
      DZ = dz;
      LeftButtonPressed = isLeftButtonPressed;
      RightButtonPressed = isRightButtonPressed;
    }
  }
}
