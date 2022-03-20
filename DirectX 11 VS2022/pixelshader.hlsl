struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);//範例 (可連續製作多個)

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    return float4(pixelColor, 1.0f);
}