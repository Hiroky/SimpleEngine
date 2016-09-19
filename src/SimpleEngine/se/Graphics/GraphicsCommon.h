﻿#pragma once

#include <windows.h>
#include <d3d11.h>

#ifndef COMPTR_RELEASE
	#define COMPTR_RELEASE(p)	if(p) { p->Release(); p = nullptr; }
#endif

#ifndef THROW_IF_FAILED
	#define THROW_IF_FAILED(hr) if (FAILED(hr)) { throw; }
#endif

namespace se
{

	/**
	 * 頂点アトリビュート
	 */
	enum VertexAttribute
	{
		VERTEX_ATTR_POSITION,
		VERTEX_ATTR_NORMAL,
		VERTEX_ATTR_COLOR,
		VERTEX_ATTR_BYTE_COLOR,
		VERTEX_ATTR_TEXCOORD0,
		VERTEX_ATTR_TEXCOORD1,
		VERTEX_ATTR_TEXCOORD2,
		VERTEX_ATTR_TEXCOORD3,
		VERTEX_ATTR_TANGENT,
		VERTEX_ATTR_BITANGENT,

		VERTEX_ATTR_UNKNOWN,
	};

	enum VertexAttributeFlag
	{
		VERTEX_ATTR_FLAG_POSITION = 1 << VERTEX_ATTR_POSITION,
		VERTEX_ATTR_FLAG_NORMAL = 1 << VERTEX_ATTR_NORMAL,
		VERTEX_ATTR_FLAG_COLOR = 1 << VERTEX_ATTR_COLOR,
		VERTEX_ATTR_FLAG_BYTE_COLOR = 1 << VERTEX_ATTR_BYTE_COLOR,
		VERTEX_ATTR_FLAG_TEXCOORD0 = 1 << VERTEX_ATTR_TEXCOORD0,
		VERTEX_ATTR_FLAG_TEXCOORD1 = 1 << VERTEX_ATTR_TEXCOORD1,
		VERTEX_ATTR_FLAG_TEXCOORD2 = 1 << VERTEX_ATTR_TEXCOORD2,
		VERTEX_ATTR_FLAG_TEXCOORD3 = 1 << VERTEX_ATTR_TEXCOORD3,
		VERTEX_ATTR_FLAG_TANGENT = 1 << VERTEX_ATTR_TANGENT,
		VERTEX_ATTR_FLAG_BITANGENT = 1 << VERTEX_ATTR_BITANGENT,
	};
	typedef uint32_t VertexAttributeFlags;

	/**
	 * プリミティブタイプ
	 */
	enum PrimitiveType
	{
		PRIMITIVE_TYPE_TRIANGLE_LIST,
		PRIMITIVE_TYPE_LINE_LIST,

		PRIMITIVE_TYPE_UNKNOWN,
	};

	/**
	 * フォーマット
	 */
	enum Format
	{
		FORMAT_UNKNOWN,
		FORMAT_R32G32B32A32_TYPELESS,
		FORMAT_R32G32B32A32_FLOAT,
		FORMAT_R32G32B32A32_UINT,
		FORMAT_R32G32B32A32_SINT,
		FORMAT_R32G32B32_TYPELESS,
		FORMAT_R32G32B32_FLOAT,
		FORMAT_R32G32B32_UINT,
		FORMAT_R32G32B32_SINT,
		FORMAT_R16G16B16A16_TYPELESS,
		FORMAT_R16G16B16A16_FLOAT,
		FORMAT_R16G16B16A16_UNORM,
		FORMAT_R16G16B16A16_UINT,
		FORMAT_R16G16B16A16_SNORM,
		FORMAT_R16G16B16A16_SINT,
		FORMAT_R32G32_TYPELESS,
		FORMAT_R32G32_FLOAT,
		FORMAT_R32G32_UINT,
		FORMAT_R32G32_SINT,
		FORMAT_R32G8X24_TYPELESS,
		FORMAT_D32_FLOAT_S8X24_UINT,
		FORMAT_R32_FLOAT_X8X24_TYPELESS,
		FORMAT_X32_TYPELESS_G8X24_UINT,
		FORMAT_R10G10B10A2_TYPELESS,
		FORMAT_R10G10B10A2_UNORM,
		FORMAT_R10G10B10A2_UINT,
		FORMAT_R11G11B10_FLOAT,
		FORMAT_R8G8B8A8_TYPELESS,
		FORMAT_R8G8B8A8_UNORM,
		FORMAT_R8G8B8A8_UNORM_SRGB,
		FORMAT_R8G8B8A8_UINT,
		FORMAT_R8G8B8A8_SNORM,
		FORMAT_R8G8B8A8_SINT,
		FORMAT_R16G16_TYPELESS,
		FORMAT_R16G16_FLOAT,
		FORMAT_R16G16_UNORM,
		FORMAT_R16G16_UINT,
		FORMAT_R16G16_SNORM,
		FORMAT_R16G16_SINT,
		FORMAT_R32_TYPELESS,
		FORMAT_D32_FLOAT,
		FORMAT_R32_FLOAT,
		FORMAT_R32_UINT,
		FORMAT_R32_SINT,
		FORMAT_R24G8_TYPELESS,
		FORMAT_D24_UNORM_S8_UINT,
		FORMAT_R24_UNORM_X8_TYPELESS,
		FORMAT_X24_TYPELESS_G8_UINT,
		FORMAT_R8G8_TYPELESS,
		FORMAT_R8G8_UNORM,
		FORMAT_R8G8_UINT,
		FORMAT_R8G8_SNORM,
		FORMAT_R8G8_SINT,
		FORMAT_R16_TYPELESS,
		FORMAT_R16_FLOAT,
		FORMAT_D16_UNORM,
		FORMAT_R16_UNORM,
		FORMAT_R16_UINT,
		FORMAT_R16_SNORM,
		FORMAT_R16_SINT,
		FORMAT_R8_TYPELESS,
		FORMAT_R8_UNORM,
		FORMAT_R8_UINT,
		FORMAT_R8_SNORM,
		FORMAT_R8_SINT,
		FORMAT_A8_UNORM,
		FORMAT_R1_UNORM,
		FORMAT_R9G9B9E5_SHAREDEXP,
		FORMAT_R8G8_B8G8_UNORM,
		FORMAT_G8R8_G8B8_UNORM,
		FORMAT_BC1_TYPELESS,
		FORMAT_BC1_UNORM,
		FORMAT_BC1_UNORM_SRGB,
		FORMAT_BC2_TYPELESS,
		FORMAT_BC2_UNORM,
		FORMAT_BC2_UNORM_SRGB,
		FORMAT_BC3_TYPELESS,
		FORMAT_BC3_UNORM,
		FORMAT_BC3_UNORM_SRGB,
		FORMAT_BC4_TYPELESS,
		FORMAT_BC4_UNORM,
		FORMAT_BC4_SNORM,
		FORMAT_BC5_TYPELESS,
		FORMAT_BC5_UNORM,
		FORMAT_BC5_SNORM,
		FORMAT_B5G6R5_UNORM,
		FORMAT_B5G5R5A1_UNORM,
		FORMAT_B8G8R8A8_UNORM,
		FORMAT_B8G8R8X8_UNORM,
		FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
		FORMAT_B8G8R8A8_TYPELESS,
		FORMAT_B8G8R8A8_UNORM_SRGB,
		FORMAT_B8G8R8X8_TYPELESS,
		FORMAT_B8G8R8X8_UNORM_SRGB,
		FORMAT_BC6H_TYPELESS,
		FORMAT_BC6H_UF16,
		FORMAT_BC6H_SF16,
		FORMAT_BC7_TYPELESS,
		FORMAT_BC7_UNORM,
		FORMAT_BC7_UNORM_SRGB,
	};


	/**
	 * 頂点バッファのストライドを算出
	 */
	inline uint32_t ComputeVertexStride(uint32_t attr)
	{
		uint32_t size = 0;
		if (attr & VERTEX_ATTR_FLAG_POSITION) size += 12;
		if (attr & VERTEX_ATTR_FLAG_NORMAL) size += 12;
		if (attr & VERTEX_ATTR_FLAG_COLOR) size += 16;
		if (attr & VERTEX_ATTR_FLAG_BYTE_COLOR) size += 4;
		if (attr & VERTEX_ATTR_FLAG_TEXCOORD0) size += 8;
		if (attr & VERTEX_ATTR_FLAG_TEXCOORD1) size += 8;
		if (attr & VERTEX_ATTR_FLAG_TEXCOORD2) size += 8;
		if (attr & VERTEX_ATTR_FLAG_TEXCOORD3) size += 8;
		if (attr & VERTEX_ATTR_FLAG_TANGENT) size += 12;
		if (attr & VERTEX_ATTR_FLAG_BITANGENT) size += 12;
		return size;
	}

}