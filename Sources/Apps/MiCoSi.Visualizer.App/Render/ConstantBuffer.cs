using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using SlimDX.Direct3D11;

namespace MiCoSi
{
  internal class Material
  {
    public SlimDX.Vector4 DiffColor { get; private set; }
    public SlimDX.Vector4 SpecColor { get; private set; }
    public SlimDX.Vector4 RimColor  { get; private set; }

    public Material()
    {
      DiffColor = new SlimDX.Vector4(1.0f);
      SpecColor = new SlimDX.Vector4(1.0f);
      RimColor = new SlimDX.Vector4(1.0f);
    }

    public Material(SlimDX.Vector4 diffColor, SlimDX.Vector4 specColor, SlimDX.Vector4 rimColor)
    {
      DiffColor = diffColor;
      SpecColor = specColor;
      RimColor = rimColor;
    }
  }

  internal class ConstantBuffer : IDisposable
  {
    private SlimDX.Direct3D11.Device _device;
    private SlimDX.Direct3D11.Buffer _constBuffer;
    private int sizeInBytes;
    private bool _wasChanged;

    private SlimDX.Matrix _world;
    private SlimDX.Matrix _view;
    private SlimDX.Matrix _projection;
    private SlimDX.Vector4 _lightDirection;
    private SlimDX.Vector4 _ambientLight;
    private SlimDX.Vector4 _cameraPosition;
    private Material _material;

    public ConstantBuffer(SlimDX.Direct3D11.Device device)
    {
      _device = device;
      sizeInBytes = 4 * 4 * sizeof(float) * 2 +
                    4 * sizeof(float) * 3 +
                    sizeof(float) * (4 + 4 + 4);
      BufferDescription constBufferDescription = new BufferDescription();
      constBufferDescription.Usage = ResourceUsage.Default;
      constBufferDescription.SizeInBytes = sizeInBytes;
      constBufferDescription.BindFlags = BindFlags.ConstantBuffer;
      constBufferDescription.CpuAccessFlags = CpuAccessFlags.None;
      _constBuffer = new SlimDX.Direct3D11.Buffer(device, constBufferDescription);
      _wasChanged = true;

      _world = SlimDX.Matrix.Identity;
      _view = SlimDX.Matrix.Identity;
      _projection = SlimDX.Matrix.Identity;
      _material = new Material(new SlimDX.Vector4(0.6f, 0.6f, 0.6f, 1.0f),
                               new SlimDX.Vector4(1.0f, 1.0f, 1.0f, 1.0f),
                               new SlimDX.Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      _lightDirection = new SlimDX.Vector4(0.0f, 0.0f, 1.0f, 1.0f);
      _ambientLight = new SlimDX.Vector4(0.1f, 0.1f, 0.1f, 1.0f);
      _cameraPosition = new SlimDX.Vector4(0.0f, 0.0f, -1.0f, 1.0f);
    }

    public SlimDX.Matrix World
    {
      get
      { return _world; }
      set
      { _world = value; _wasChanged = true; }
    }

    public SlimDX.Matrix View
    {
      get
      { return _view; }
      set
      { _view = value; _wasChanged = true; }
    }

    public SlimDX.Matrix Projection
    {
      get
      { return _projection; }
      set
      { _projection = value; _wasChanged = true; }
    }

    public SlimDX.Vector4 LightDirection
    {
      get
      { return _lightDirection; }
      set
      { _lightDirection = value; _wasChanged = true; }
    }

    public SlimDX.Vector4 AmbientLight
    {
      get
      { return _ambientLight; }
      set
      { _ambientLight = value; _wasChanged = true; }
    }

    public SlimDX.Vector4 CameraPosition
    {
      get
      { return _cameraPosition; }
      set
      { _cameraPosition = value; _wasChanged = true; }
    }

    public Material Material
    {
      get
      { return _material; }
      set
      { _material = value; _wasChanged = true; }
    }

    public SlimDX.Direct3D11.Buffer Buffer
    {
      get
      {
        if (_wasChanged)
        {
          SlimDX.DataStream stream = new SlimDX.DataStream(sizeInBytes, true, true);
          stream.Write(_world);
          stream.Write(_projection *_view * _world);
          stream.Write(_lightDirection);
          stream.Write(_ambientLight);
          stream.Write(_cameraPosition);
          stream.Write(_material.DiffColor);
          stream.Write(_material.SpecColor);
          stream.Write(_material.RimColor);
          stream.Position = 0;
          _device.ImmediateContext.UpdateSubresource(new SlimDX.DataBox(0, 0, stream), _constBuffer, 0);
          stream.Dispose();
          _wasChanged = false;
        }

        return _constBuffer;
      }
    }

    public void Dispose()
    {
      if (_constBuffer != null)
      {
        _constBuffer.Dispose();
        _constBuffer = null;
      }

      _device = null;
    }
  }
}
