using SlimDX.Direct3D11;

namespace Mitosis.DxRender
{
  internal interface IRenderTargetAspect
  {
    float AspectRatio { get; }
    void GetTargets(out DepthStencilView depthStencilView, out RenderTargetView renderTargetView, out Viewport viewport);
    void OnCompleteRender();
  }
}
