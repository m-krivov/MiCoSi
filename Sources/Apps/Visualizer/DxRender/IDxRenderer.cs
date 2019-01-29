using System;

namespace Mitosis.DxRender
{
  /// <summary>
  /// Renders cell state onto required surface
  /// </summary>
  internal interface IDxRenderer : IDisposable
  {
    /// <summary>
    /// Draws cell as static structure
    /// </summary>
    void Draw(DxRenderContext context, Cell cell, SimParams parameters);

    /// <summary>
    /// Returns object that allows to change camera settings
    /// </summary>
    Camera Camera { get; set; }

    /// <summary>
    /// Just clears render area
    /// </summary>
    void Clear(DxRenderContext context);
  }
}
