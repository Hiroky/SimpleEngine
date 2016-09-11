//
// Copyright (c) GANBARION Co., Ltd. All rights reserved.
// This code is licensed under the MIT License (MIT).
//

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

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

PSInput VS(Vertex v)
{
	PSInput o;
	o.v_position = v.a_position;
	o.v_texcoord0 = v.a_texcoord0;
	return o;
}

float4 PS(PSInput i) : SV_Target
{
	return texture0.Sample(sampler0, i.v_texcoord0);
}
