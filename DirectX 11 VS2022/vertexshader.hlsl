cbuffer mycBuffer : register(b0)
{
    float xOffset;
    float yOffset;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float3 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    input.inPos.x += xOffset;
    input.inPos.y += yOffset;
    output.outPosition = float4(input.inPos, 10.0f);
    output.outTexCoord = input.inTexCoord;
    return output;
}