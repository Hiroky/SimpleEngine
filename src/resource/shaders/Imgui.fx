

struct VS_INPUT {
    float3 pos: POSITION;
    float4 col: BYTECOLOR0;
    float2 uv: TEXCOORD0;
};

struct PS_INPUT {
    float4 pos: SV_POSITION;
    float4 col: COLOR0;
    float2 uv: TEXCOORD0;
};


cbuffer Constants : register(b0) 
{
    float4x4 ProjectionMatrix;
};

SamplerState sampler0;
Texture2D texture0;


PS_INPUT mainVS(VS_INPUT input) 
{
    PS_INPUT output;
    output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.0f, 1.0f));
    output.col = input.col;
    output.uv = input.uv;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * texture0.Sample(sampler0, input.uv); 
    return out_col; 
}

