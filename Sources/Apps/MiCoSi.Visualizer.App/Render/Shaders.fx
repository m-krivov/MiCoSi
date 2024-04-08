
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix WorldViewProjection;
    float4 LightDirection;
    float4 AmbientLight;
    float4 CameraPosition;
    float4 DiffColor;
    float4 SpecColor;
    float4 RimColor;
}

struct VS_INPUT
{
    float3 Pos  : POSITION;
    float3 Norm : NORMAL;
};

struct PS_INPUT
{
    float4 SvPos  : SV_POSITION;
    float4 Pos    : TEXCOORD0;
    float3 Norm   : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input )
{
    PS_INPUT output;

    output.SvPos = mul(float4(input.Pos, 1.0), WorldViewProjection);
    output.Pos   = mul(float4(input.Pos, 1.0), World);
    output.Norm  = mul(input.Norm, (float3x3)World);
 
    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
   float3 normal = normalize(input.Norm);
   float3 toLightSource = normalize(-LightDirection.xyz);
   float3 toCamera = normalize(CameraPosition.xyz - input.Pos);
   float3 h = normalize(toCamera + toLightSource);
   float3 res = AmbientLight.xyz;
   res += DiffColor.xyz * max(dot(normal, toLightSource), 0.0);
   res += SpecColor.xyz * pow(max(dot(normal, h), 0.0), 30.0);
   res += RimColor.xyz * pow(1.3 - max(dot(normal, toCamera), 0.0), 8);
   return float4(res, 1.0);
}

float4 PS_solid(PS_INPUT input) : SV_Target
{
   return float4(DiffColor.xyz, 1.0);
}


technique10 Default
{
    pass P0
        {
            SetGeometryShader(0);
            SetVertexShader(CompileShader(vs_4_0, VS()));
            SetPixelShader(CompileShader(ps_4_0, PS()));
        }
}

technique10 Solid
{
    pass P0
        {
            SetGeometryShader(0);
            SetVertexShader(CompileShader(vs_4_0, VS()));
            SetPixelShader(CompileShader(ps_4_0, PS_solid()));
        }
}

