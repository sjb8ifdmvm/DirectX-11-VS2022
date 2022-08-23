cbuffer lightBuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

Texture2D<float3> objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);//範例 (可連續製作多個)

float4 main(PS_INPUT input) : SV_TARGET
{
    //float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    
    float3 sampleColor = input.inNormal;
    
    float3 ambientLight = ambientLightColor * ambientLightStrength;

    float3 finalColor = sampleColor * ambientLight;

    return float4(finalColor, 1.0f);
}