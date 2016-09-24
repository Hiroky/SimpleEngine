#include "Common.h"

struct Vertex
{
	float4 a_position : POSITION;
	float2 a_texcoord0: TEXCOORD0;
};

struct PSInput
{
	float4 v_position : SV_POSITION;
	float2 v_texcoord0: TEXCOORD0;
};

Texture2D currentBuffer : register(t0);
Texture2D historyBuffer : register(t1);
SamplerState linearSampler : register(s0);


void NeighborhoodMinMax(float2 pos, out float4 current, out float4 neighborMin, out float4 neighborMax)
{
	float4 neighbor0 = currentBuffer.Load(int3(pos.xy, 0), float2(-1, -1));
	float4 neighbor1 = currentBuffer.Load(int3(pos.xy, 0), float2( 0, -1));
	float4 neighbor2 = currentBuffer.Load(int3(pos.xy, 0), float2( 1, -1));
	float4 neighbor3 = currentBuffer.Load(int3(pos.xy, 0), float2(-1,  0));
	float4 neighbor4 = currentBuffer.Load(int3(pos.xy, 0), float2( 0,  0));
	float4 neighbor5 = currentBuffer.Load(int3(pos.xy, 0), float2( 1,  0));
	float4 neighbor6 = currentBuffer.Load(int3(pos.xy, 0), float2(-1,  1));
	float4 neighbor7 = currentBuffer.Load(int3(pos.xy, 0), float2( 0,  1));
	float4 neighbor8 = currentBuffer.Load(int3(pos.xy, 0), float2( 1,  1));
	current = neighbor4;
	neighborMin = min(neighbor0, min(neighbor1, min(neighbor2, min(neighbor3, min(neighbor4, min(neighbor5, min(neighbor6, min(neighbor7, neighbor8))))))));
	neighborMax = max(neighbor0, max(neighbor1, max(neighbor2, max(neighbor3, max(neighbor4, max(neighbor5, max(neighbor6, max(neighbor7, neighbor8))))))));
}

PSInput VS(Vertex v)
{
	PSInput o;
	o.v_position = v.a_position;
	o.v_texcoord0 = v.a_texcoord0;
	return o;
}

float4 PS(PSInput i) : SV_Target
{
	float4 current, min, max;
	NeighborhoodMinMax(i.v_position.xy, current, min, max);
	float4 history = historyBuffer.SampleLevel(linearSampler, i.v_texcoord0, 0);
	history = clamp(history, min, max);
	return lerp(history, current, 0.05);
}
