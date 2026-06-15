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

    float3 v_worldSpaceTangent : TANGENT;
    float3 v_worldSpaceBitangent : BITANGENT;
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

float3x3 BuildTangentToWorldTransform(float3 interpolatedTangent, float3 interpolatedBitangent, float3 interpolatedNormal)
{
    float3 worldSpaceNormal = normalize(interpolatedNormal);
    
    float3 worldSpaceTangent = interpolatedTangent - dot(interpolatedTangent, worldSpaceNormal) * worldSpaceNormal;
    worldSpaceNormal = normalize(worldSpaceNormal);
    
    float3 worldSpaceBitangent = cross(worldSpaceNormal, worldSpaceTangent);
    
    if (dot(worldSpaceTangent, interpolatedBitangent) < 0.f)
    {
        worldSpaceBitangent = -worldSpaceBitangent;
    }
    worldSpaceBitangent = normalize(worldSpaceBitangent);
    
    return float3x3(worldSpaceTangent, worldSpaceBitangent, worldSpaceNormal);
}

v2p_t VertexMain(vs_input_t input)
{
    float4 modelSpacePosition = float4(input.a_modelSpacePosition, 1.f);

    float4 worldSpacePosition = mul(u_modelToWorld, modelSpacePosition);
    float4 cameraSpacePosition = mul(u_worldToCamera, worldSpacePosition);
    float4 renderSpacePosition = mul(u_cameraToRender, cameraSpacePosition);
    float4 clipSpacePosition = mul(u_renderToClip, renderSpacePosition);

    float4 modelSpaceTangent = float4(input.a_tangent, 0.f);
    float4 modelSpaceBitangent = float4(input.a_bitangent, 0.f);
    float4 modelSpaceNormal = float4(input.a_normal, 0.f);

    float3 worldSpaceTangent = normalize(mul(u_modelToWorld, modelSpaceTangent).xyz);
    float3 worldSpaceBitangent = normalize(mul(u_modelToWorld, modelSpaceBitangent).xyz);
    float3 worldSpaceNormal = normalize(mul(u_modelToWorld, modelSpaceNormal).xyz);

    v2p_t v2p;
    v2p.v_clipSpacePosition = clipSpacePosition;
    v2p.v_color = input.a_color;
    v2p.v_uv = input.a_uv;

    v2p.v_worldSpaceTangent = worldSpaceTangent;
    v2p.v_worldSpaceBitangent = worldSpaceBitangent;
    v2p.v_worldSpaceNormal = worldSpaceNormal;
    

    return v2p;
}

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState diffuseSampler : register(s0);
SamplerState normalSampler : register(s1);

float4 PixelMain(v2p_t input) : SV_Target0
{
    float4 diffuseTexel = diffuseTexture.Sample(diffuseSampler, input.v_uv);
    float4 normalTexel = normalTexture.Sample(normalSampler, input.v_uv);

    float4 vertexColor = input.v_color;
    float4 modelTint = u_modelTint;

    float4 diffuseColor = diffuseTexel * vertexColor * modelTint;

    clip(diffuseColor.a - 0.01f);

    float3 surfaceWorldTangent = normalize(input.v_worldSpaceTangent);
    float3 surfaceWorldBitangent = normalize(input.v_worldSpaceBitangent);
    float3 surfaceWorldNormal = normalize(input.v_worldSpaceNormal);

    float3 tangentSpaceNormal = DecodeColorAsSignedVector(normalTexel.rgb);
    tangentSpaceNormal = normalize(tangentSpaceNormal);

    float3x3 tangentToWorldMatrix = BuildTangentToWorldTransform(
        input.v_worldSpaceTangent,
        input.v_worldSpaceBitangent,
        input.v_worldSpaceNormal
    );

    float3 pixelWorldNormal = mul(tangentSpaceNormal, tangentToWorldMatrix);
    pixelWorldNormal = normalize(pixelWorldNormal);

    float3 pixelToLightDirection = normalize(-u_sunDirection);

    float surfaceLightingDot = dot(surfaceWorldNormal, pixelToLightDirection);
    surfaceLightingDot = saturate(surfaceLightingDot);

    float normalMappedLightingDot = dot(pixelWorldNormal, pixelToLightDirection);
    normalMappedLightingDot = saturate(normalMappedLightingDot);

    if (c_debugInt == 1)
    {
        return diffuseTexel;
    }
    else if (c_debugInt == 2)
    {
        return normalTexel;
    }
    else if (c_debugInt == 3)
    {
        return float4(input.v_uv.x, input.v_uv.y, 0.f, diffuseColor.a);
    }
    else if (c_debugInt == 4)
    {
        return float4(EncodeSignedVectorAsColor(surfaceWorldTangent), diffuseColor.a);
    }
    else if (c_debugInt == 5)
    {
        return float4(EncodeSignedVectorAsColor(surfaceWorldBitangent), diffuseColor.a);
    }
    else if (c_debugInt == 6)
    {
        return float4(EncodeSignedVectorAsColor(surfaceWorldNormal), diffuseColor.a);
    }
    else if (c_debugInt == 7)
    {
        return float4(EncodeSignedVectorAsColor(pixelWorldNormal), diffuseColor.a);
    }
    else if (c_debugInt == 8)
    {
        return float4(surfaceLightingDot.xxx, diffuseColor.a);
    }
    else if (c_debugInt == 9)
    {
        return float4(normalMappedLightingDot.xxx, diffuseColor.a);
    }

    float lightAmount = max(normalMappedLightingDot * u_sunIntensity, u_ambientIntensity);
    float3 litColor = diffuseColor.rgb * lightAmount;

    return float4(litColor, diffuseColor.a);
}