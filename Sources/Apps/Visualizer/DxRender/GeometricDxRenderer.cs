using System;
using System.Collections.Generic;
using SlimDX;
using SlimDX.Direct3D11;
using SlimDX.DXGI;

namespace Mitosis.DxRender
{
  internal class GeometricDxRenderer : AbstractDxRenderer
  {
    public GeometricDxRenderer(DxDeviceContext deviceContext)
      : base(deviceContext, new Camera(new Vector3(0.0f, 0.0f, -2.0f),
                                       new Vector3(0.0f, 0.0f, 0.0f),
                                       new Vector3(0.0f, 1.0f, 0.0f)))
    {
      Meshes.Register("tube", MeshGenerator.CreateTube(1.0f, 1.0f, new Vector3(0, 0, -1)));
      Meshes.Register("sphere", MeshGenerator.CreateSphere(1.0f));
      Meshes.Register("emptyHalfSphere", MeshGenerator.CreateEmptyHalfSphere(1.0f, new Vector3(0, 0, -1)));
      Meshes.Register("halfCylinder", MeshGenerator.CreateHalfCylinder(1.0f, 1.0f, new Vector3(0, 1, 0), -(float)Math.PI / 2));
      Meshes.Register("rod", MeshGenerator.CreateTube(1.0f, 0.5f, new Vector3(0, 0, -1)));
      Meshes.Register("spring", MeshGenerator.CreateSpring(0.25f, 1.0f, new Vector3(0, 0, -1), 10, 5, 0.05f, 6));
    }

    protected override void DrawCore(DxRenderContext renderContext, Cell cell, SimParams parameters)
    {
      EffectPass pass
        = DeviceContext.Effect.GetTechniqueByName(renderContext.StyleAspect.TechniqueName).GetPassByName("P0");
      pass.Apply(DeviceContext.Device.ImmediateContext);

      if (cell != null)
      {
        using (InputLayout layout
              = new InputLayout(DeviceContext.Device, pass.Description.Signature, new[]
                        { new InputElement("POSITION", 0, Format.R32G32B32_Float, 0, 0),
                          new InputElement("NORMAL", 0, Format.R32G32B32_Float, 12, 0) }))
        {
          float normalizer = 0.8f / (float)parameters.GetValue(SimParameter.Double.R_Cell, true);

          DeviceContext.Device.ImmediateContext.InputAssembler.InputLayout = layout;
          DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology = PrimitiveTopology.TriangleList;

          DrawCell(renderContext, cell, parameters, normalizer);
          DrawTubes(renderContext, cell, parameters, normalizer);
          DrawPoles(renderContext, cell, normalizer);
          DrawChromosomes(renderContext, cell, parameters, normalizer);
          DrawSprings(renderContext, cell, parameters, normalizer);
        }
      }
    }

    private static Matrix FromMat3x3D(Mat3x3D mat)
    {
      Matrix res = new Matrix();
      res.set_Rows(0, new Vector4((float)mat[0, 0], (float)mat[1, 0], (float)mat[2, 0], 0.0f));
      res.set_Rows(1, new Vector4((float)mat[0, 1], (float)mat[1, 1], (float)mat[2, 1], 0.0f));
      res.set_Rows(2, new Vector4((float)mat[0, 2], (float)mat[1, 2], (float)mat[2, 2], 0.0f));
      res.set_Rows(3, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
      return res;
    }

    private static Matrix RotationVectors(Vector3 v1, Vector3 v2)
    {
      v1.Normalize();
      v2.Normalize();

      Vector3 cp = Vector3.Cross(v1, v2);

      if (cp.Length() < 1e-3)
      {
        Vector3[] orts = { new Vector3(1, 0, 0), new Vector3(0, 1, 0), new Vector3(0, 0, 1) };

        foreach (Vector3 ort in orts)
        {
          cp = Vector3.Cross(v1, ort);

          if (cp.Length() > 1e-1)
          {
            break;
          }
        }
      }

      cp.Normalize();

      return Matrix.RotationAxis(cp, (float)Math.Acos(Vector3.Dot(v1, v2)));
    }

    private void DrawCell(DxRenderContext renderContext, Cell cell, SimParams parameters, float normalizer)
    {
      SlimDX.Direct3D11.Device device = DeviceContext.Device;
      DxMesh shSphere = Meshes.GetMesh("emptyHalfSphere");
      ConstBuffer.Material = renderContext.StyleAspect.Resolve<CellStyle>(cell).Cell;

      Vector3 oldPos = Camera.Position;
      Vector3 oldUp = Camera.Up;
      Vector4 oldLightDirection = ConstBuffer.LightDirection;
      Camera.Position = new Vector3(0.0f, 0.0f, -oldPos.Length());
      Camera.Up = new Vector3(0.0f, 1.0f, 0.0f);
      ConstBuffer.LightDirection = new Vector4(Camera.Direction, 1.0f);
      ConstBuffer.View = Matrix.Transpose(Camera.ViewMatrix);

      float radius = (float)parameters.GetValue(SimParameter.Double.R_Cell, true) * normalizer;
      float dist = oldPos.Length();
      float magicCoeff = dist / (float)Math.Sqrt(Math.Max(0.1, dist * dist - radius * radius));
      Matrix transform = Matrix.Scaling(radius * magicCoeff, radius * magicCoeff, radius * magicCoeff);

      transform *= RotationVectors(new Vector3(0.0f, 0.0f, 1.0f), new Vector3(0.0f, 0.0f, -1.0f));

      ConstBuffer.World = Matrix.Transpose(transform);
      SetBuffer(device, ConstBuffer);
      SetAndDrawMesh(device, shSphere);
      ClearDepthStencil();

      Camera.Position = oldPos;
      Camera.Up = oldUp;
      ConstBuffer.LightDirection = oldLightDirection;
      ConstBuffer.View = Matrix.Transpose(Camera.ViewMatrix);
    }

    private void DrawTubes(DxRenderContext renderContext, Cell cell, SimParams parameters, float normalizer)
    {
      float kin_d = (float)parameters.GetValue(SimParameter.Double.Cr_Kin_D, true) * normalizer;
      float kin_l = (float)parameters.GetValue(SimParameter.Double.Cr_Kin_L, true) * normalizer;

      SlimDX.Direct3D11.Device device = DeviceContext.Device;
      DxMesh tube = Meshes.GetMesh("tube");
      SetMesh(device, tube);

      // Sorting tubes.
      List<MT>[] tubes = { new List<MT>(),
                           new List<MT>(),
                           new List<MT>(),
                           new List<MT>() };

      Func<TubeStyle, Material>[] materialSelectors = {
        style => style.FreeLeftPoleTube,
        style => style.BoundLeftPoleTube,
        style => style.FreeRightPoleTube,
        style => style.BoundRightPoleTube
      };

      foreach (MT mt in cell.MTs)
      {
        int index = (mt.BoundChromosome == null ? 0 : 1) +
                    (mt.Pole.Type == PoleType.Left ? 0 : 2);
        tubes[index].Add(mt);
      }

      var prevMode = DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology;
      PrimitiveTopology[] modes = { PrimitiveTopology.LineList, PrimitiveTopology.TriangleList };

      for (int j = 0; j < modes.Length; j++)
      {
        DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology = modes[j];

        for (int i = 0; i < 4; i++)
        {
          foreach (MT mt in tubes[i])
          {
            TubeStyle tubeStyle = renderContext.StyleAspect.Resolve<TubeStyle>(mt);
            ConstBuffer.Material = materialSelectors[i](tubeStyle);

            // Converting anchor points.
            var _pole = mt.Pole.Position;
            var pole = new Vector3((float)_pole.X * normalizer,
                                   (float)_pole.Y * normalizer,
                                   (float)_pole.Z * normalizer);
            var _force = mt.ForcePoint;
            var force = new Vector3((float)_force.X * normalizer,
                                    (float)_force.Y * normalizer,
                                    (float)_force.Z * normalizer);
            var _end = mt.EndPoint;
            var end = new Vector3((float)_end.X * normalizer,
                                  (float)_end.Y * normalizer,
                                  (float)_end.Z * normalizer);

            // Generating list with curve's points.
            List<Vector3> points = new List<Vector3>();
            points.Add(pole);
            points.Add(force);
            if (force != end && mt.BoundChromosome != null)
            {
              // Building new 2D coordinate system.
              Vec3D _center = mt.BoundChromosome.Position;
              Vector3 center = new Vector3((float)_center.X * normalizer,
                                           (float)_center.Y * normalizer,
                                           (float)_center.Z * normalizer);
              Matrix orient = FromMat3x3D(mt.BoundChromosome.Orientation);
              Vector4 _r1 = Vector3.Transform(new Vector3(0.0f, 0.0f, -kin_d / 2), orient);
              Vector3 r1 = new Vector3(_r1.X, _r1.Y, _r1.Z);
              Vector4 _r2 = Vector3.Transform(new Vector3(kin_d / 2, 0.0f, 0.0f), orient);
              Vector3 r2 = new Vector3(_r2.X, _r2.Y, _r2.Z);
              Vector3 ort1 = r1; ort1.Normalize();
              Vector3 ort2 = r2; ort2.Normalize();
              Vector4 _ort3 = Vector3.Transform(new Vector3(0.0f, 1.0f, 0.0f), orient);
              Vector3 ort3 = new Vector3(_ort3.X, _ort3.Y, _ort3.Z);

              // Building plane for point planting.
              Func<Vector3, Vector3> normalize
                = (e =>
                {
                  float dr1 = Vector3.Dot(ort1, e);
                  float dr2 = Vector3.Dot(ort2, e);
                  float l = (float)Math.Sqrt(dr1 * dr1 + dr2 * dr2);
                  return e * (kin_d / 2 / l);
                });

              Vector3 normal = Vector3.Cross(end - pole, force - pole);
              if (normal.Length() < (float)1e-3) normal = ort3;
              else normal.Normalize();
              float t = -(normal.X * (center.X - end.X) +
                          normal.Y * (center.Y - end.Y) +
                          normal.Z * (center.Z - end.Z)) /
                         (normal.X * ort3.X + normal.Y * ort3.Y + normal.Z * ort3.Z);
              Vector3 proj_center = center + ort3 * (float)Math.Min(kin_l / 2, Math.Max(-kin_l / 2, t));

              // Selection direction - clockwise or not
              float l1 = 0.0f, l2 = 0.0f;
              {
                float dx = Vector3.Dot(end - center, ort2), dy = Vector3.Dot(end - center, ort1);
                float dl = (float)Math.Sqrt(dx * dx + dy * dy);
                float angle1 = (float)(Math.PI - Math.Asin(dy / dl));

                dx = Vector3.Dot(force - center, ort2); dy = Vector3.Dot(force - center, ort1);
                dl = (float)Math.Sqrt(dx * dx + dy * dy);
                float angle2 = (float)(Math.PI - Math.Asin(dy / dl));

                l1 = (float)(Math.Abs(angle2 - angle1) * kin_d / 2);
                l2 = (float)(Math.Abs(Math.Min(angle1, angle2) - Math.PI / 2) * kin_d / 2 +
                             Math.Abs(Math.Max(angle1, angle2) - 3 * Math.PI / 2) * kin_d / 2 +
                             kin_d);
              }
              if (l1 < l2)
              {
                // By curve.
                Vector3 v1 = force - proj_center;
                Vector3 v5 = end - proj_center;
                Vector3 v3 = normalize(v1 + v5);
                Vector3 v2 = normalize(v1 + v3);
                Vector3 v4 = normalize(v3 + v5);

                points.Add(proj_center + v2 * 1.02f);
                points.Add(proj_center + v3 * 1.02f);
                points.Add(proj_center + v4 * 1.02f);
                points.Add(proj_center + v5 * 1.02f);
              }
              else
              {
                // By back plane.
                Vector3 v2 = Vector3.Dot(force, ort1) > 0.0 ? -r1 : r1;
                v2 = normalize(v2);         //  don't work! Need to be fixed!
                Vector3 v3 = Vector3.Dot(force, ort1) > 0.0 ? r1 : -r1;
                v3 = normalize(v3);         //  don't work! Need to be fixed!
                Vector3 v4 = end - proj_center;

                points.Add(proj_center + v2 * 1.02f);
                points.Add(proj_center + v3 * 1.02f);
                points.Add(proj_center + v4 * 1.02f);
              }
            }

            // Drawing generated segments.
            for (int k = 0; k < points.Count - 1; k++)
            {
              var dir = points[k + 1] - points[k];
              float scale = dir.Length();
              Matrix transform = Matrix.Scaling((float)tubeStyle.Width,
                                                (float)tubeStyle.Width,
                                                scale);

              transform *= RotationVectors(new Vector3(0, 0, -1), dir);
              transform *= Matrix.Translation(points[k].X,
                                              points[k].Y,
                                              points[k].Z);

              ConstBuffer.World = Matrix.Transpose(transform);
              SetBuffer(device, ConstBuffer);
              DrawMesh(device, tube);
            }
          }
        }
      }

      DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology = prevMode;
    }

    private void DrawPoles(DxRenderContext renderContext, Cell cell, float normalizer)
    {
      SlimDX.Direct3D11.Device device = DeviceContext.Device;
      DxMesh sphere = Meshes.GetMesh("sphere");

      SetMesh(device, sphere);

      Pole[] poles = { cell.GetPole(PoleType.Left), cell.GetPole(PoleType.Right) };
      Material[] materials = { renderContext.StyleAspect.Resolve<PoleStyle>(poles[0]).LeftPole, renderContext.StyleAspect.Resolve<PoleStyle>(poles[1]).RightPole };

      for (int i = 0; i < 2; i++ )
      {
        ConstBuffer.Material = materials[i];
        Pole pole = poles[i];
        float radius = (float)renderContext.StyleAspect.Resolve<PoleStyle>(pole).Radius;
        Matrix transform = Matrix.Scaling(radius, radius, radius);
        transform *= Matrix.Translation((float)pole.Position.X * normalizer,
                                        (float)pole.Position.Y * normalizer,
                                        (float)pole.Position.Z * normalizer);
        ConstBuffer.World = Matrix.Transpose(transform);
        SetBuffer(device, ConstBuffer);
        DrawMesh(device, sphere);
      }
    }

    private void DrawSprings(DxRenderContext renderContext, Cell cell, SimParams parameters, float normalizer)
    {
      SlimDX.Direct3D11.Device device = DeviceContext.Device;
      DxMesh tube = parameters.GetValue(SimParameter.Int.Spring_Type) == 0
            ? Meshes.GetMesh("rod") : Meshes.GetMesh("spring");
      SetMesh(device, tube);

      var prevMode = DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology;
      PrimitiveTopology[] modes = { PrimitiveTopology.LineList, PrimitiveTopology.TriangleList };

      for (int j = 0; j < modes.Length; j++)
      {
        DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology = modes[j];

        foreach (ChromosomePair pair in cell.ChromosomePairs)
        {
          Spring spring = pair.Spring;
          if (spring != null)
          {
            SpringStyle springStyle = renderContext.StyleAspect.Resolve<SpringStyle>(spring);
            ConstBuffer.Material = springStyle.Spring;
            float scale = (float)spring.Length * normalizer;
            Matrix transform = Matrix.Scaling((float)springStyle.Width, (float)springStyle.Width, scale);

            transform *= RotationVectors(new Vector3(0, 0, -1),
                                         new Vector3((float)(spring.RightJoint.X - spring.LeftJoint.X),
                                                     (float)(spring.RightJoint.Y - spring.LeftJoint.Y),
                                                     (float)(spring.RightJoint.Z - spring.LeftJoint.Z)));
            transform *= Matrix.Translation((float)spring.LeftJoint.X * normalizer,
                                            (float)spring.LeftJoint.Y * normalizer,
                                            (float)spring.LeftJoint.Z * normalizer);

            ConstBuffer.World = Matrix.Transpose(transform);
            SetBuffer(device, ConstBuffer);
            DrawMesh(device, tube);
          }
        }
      }
      DeviceContext.Device.ImmediateContext.InputAssembler.PrimitiveTopology = prevMode;
    }

    private void DrawChromosomes(DxRenderContext renderContext, Cell cell, SimParams parameters, float normalizer)
    {
      SlimDX.Direct3D11.Device device = DeviceContext.Device;
      DxMesh halfCylinder = Meshes.GetMesh("halfCylinder");

      SetMesh(device, halfCylinder);

      float kinScaleX = (float)parameters.GetValue(SimParameter.Double.Cr_Kin_D, true) / 2 * normalizer;
      float kinScaleY = (float)parameters.GetValue(SimParameter.Double.Cr_Kin_L, true) * normalizer;
      float handScaleX = (float)parameters.GetValue(SimParameter.Double.Cr_Hand_D, true) / 2 * normalizer;
      float handScaleY = (float)(parameters.GetValue(SimParameter.Double.Cr_L, true) -
                                 parameters.GetValue(SimParameter.Double.Cr_Kin_L, true)) / 2 * normalizer;
      float handOffsetY1 = (float)parameters.GetValue(SimParameter.Double.Cr_Kin_L, true) / 2 * normalizer + handScaleY;
      float handOffsetY2 = -(float)parameters.GetValue(SimParameter.Double.Cr_Kin_L, true) / 2 * normalizer;

      float[] scaleX  = { kinScaleX, handScaleX, handScaleX };
      float[] scaleY  = { kinScaleY, handScaleY, handScaleY };
      float[] offsetY = { kinScaleY / 2, handOffsetY1, handOffsetY2 };
      Func<ChromosomeStyle, Material>[] materialSelectors = { style => style.Kinetohore, style => style.Hand, style => style.Hand };

      foreach (var chr in cell.Chromosomes)
      {
        for (int i = 0; i < 3; i++)
        {
          ConstBuffer.Material = materialSelectors[i](renderContext.StyleAspect.Resolve<ChromosomeStyle>(chr));
          Matrix transform = Matrix.Scaling(scaleX[i], scaleY[i], scaleX[i]);
          transform *= Matrix.Translation(0.0f, offsetY[i], 0.0f);

          transform *= FromMat3x3D(chr.Orientation);

          transform *= Matrix.Translation((float)chr.Position.X * normalizer,
                                          (float)chr.Position.Y * normalizer,
                                          (float)chr.Position.Z * normalizer);

          ConstBuffer.World = Matrix.Transpose(transform);
          SetBuffer(device, ConstBuffer);
          DrawMesh(device, halfCylinder);
        }
      }
    }

    public override void Clear(DxRenderContext renderContext)
    {
      Draw(renderContext, null, null);
    }

    public override void Dispose()
    {
      base.Dispose();
    }
  }
}
