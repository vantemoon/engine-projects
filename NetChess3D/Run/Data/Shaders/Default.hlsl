cbuffer PerFrameConstants : register(b1)
{
    float c_time;
    int c_debugInt;
    float c_debugFloat;
    float c_perFramePadding;
};

cbuffer CameraConstants : register(b2)
{
    float4x4 u_worldToCamera;
    float4x4 u_cameraToRender;
    float4x4 u_renderToClip;
};

cbuffer ModelConstants : register(b3)
{
    float4x4 u_modelToWorld;
    float4 u_modelTint;
};

cbuffer LightingConstants : register(b4)
{
    float3 u_sunDirection;
    float u_sunIntensity;

    float u_ambientIntensity;
    float3 u_lightingPadding;
};

struct vs_input_t
{
    float3 a_modelSpacePosition : POSITION;
    float4 a_color : COLOR;
    float2 a_uv : TEXCOORD;

    float3 a_tangent : TANGENT;
    float3 a_bitangent : BITANGENT;
    float3 a_normal : NORMAL;

    uint a_vertexID : SV_VertexID;
};

struct v2p_t
{
    float4 v_clipSpacePosition : SV_Position;
    float4 v_color : COLOR;
    float2 v_uv : TEXCOORD;
    float3 v_worldSpaceNormal : NORMAL;
};

float3 EncodeSignedVectorAsColor(float3 xyzVector)
{
    return xyzVector * 0.5f + 0.5f;
}

float3 DecodeColorAsSignedVector(float3 rgbColor)
{
    return rgbColor * 2.f - 1.f;
}

v2p_t VertexMain(vs_input_t input)
{
    float4 modelSpacePosition = float4(input.a_modelSpacePosition, 1.f);

    float4 worldSpacePosition = mul(u_modelToWorld, modelSpacePosition);
    float4 cameraSpacePosition = mul(u_worldToCamera, worldSpacePosition);
    float4 renderSpacePosition = mul(u_cameraToRender, cameraSpacePosition);
    float4 clipSpacePosition = mul(u_renderToClip, renderSpacePosition);

    float4 modelSpaceNormal = float4(input.a_normal, 0.f);
    float3 worldSpaceNormal = mul(u_modelToWorld, modelSpaceNormal).xyz;

    v2p_t v2p;
    v2p.v_clipSpacePosition = clipSpacePosition;
    v2p.v_color = input.a_color;
    v2p.v_uv = input.a_uv;
    v2p.v_worldSpaceNormal = worldSpaceNormal;

    return v2p;
}

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

float4 PixelMain(v2p_t input) : SV_Target0
{
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.v_uv);
    float4 vertexColor = input.v_color;
    float4 modelTint = u_modelTint;

    float4 diffuseColor = textureColor * vertexColor * modelTint;

    clip(diffuseColor.a - 0.01f);

    float3 worldNormal = normalize(input.v_worldSpaceNormal);

    float3 pixelToLightDirection = normalize(-u_sunDirection);

    float directLightAmount = dot(worldNormal, pixelToLightDirection);
    directLightAmount = saturate(directLightAmount);

    float lightAmount = max(directLightAmount * u_sunIntensity, u_ambientIntensity);

    float3 litColor = diffuseColor.rgb * lightAmount;

    return float4(litColor, diffuseColor.a);
}