using System;

namespace Mitosis.DxRender
{
  internal class DxRenderContext : IDisposable
  {
    public DxRenderContext(IStyleAspect styleAspect, IRenderTargetAspect renderTargetAspect)
    {
      RenderTargetAspect = renderTargetAspect;
      StyleAspect = styleAspect;
    }

    public IRenderTargetAspect RenderTargetAspect { get; private set; }

    public IStyleAspect StyleAspect { get; private set; }

    public void Dispose()
    {
      var renderTargetAspect = RenderTargetAspect as IDisposable;

      if (renderTargetAspect != null)
      {
        renderTargetAspect.Dispose();
      }

      var styleAspect = StyleAspect as IDisposable;

      if (styleAspect != null)
      {
        styleAspect.Dispose();
      }
    }
  }
}
