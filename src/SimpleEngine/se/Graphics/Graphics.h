#pragma once

#ifndef COMPTR_RELEASE
	#define COMPTR_RELEASE(p)		if(p) { p->Release(); p = nullptr; }
#endif

#ifndef THROW_IF_FAILED
	#define THROW_IF_FAILED(hr) if (FAILED(hr)) { throw; }
#endif

namespace se
{

	// 頂点アトリビュート
	enum VertexAttribute
	{
		VERTEX_ATTR_POSITION		= 1 << 0,
		VERTEX_ATTR_NORMAL			= 1 << 1,
		VERTEX_ATTR_COLOR			= 1 << 2,
		VERTEX_ATTR_TEXCOORD0		= 1 << 3,
		VERTEX_ATTR_TEXCOORD1		= 1 << 4,
		VERTEX_ATTR_TEXCOORD2		= 1 << 5,
		VERTEX_ATTR_TEXCOORD3		= 1 << 6,
		VERTEX_ATTR_TANGENT			= 1 << 7,
		VERTEX_ATTR_BITANGENT		= 1 << 8,
		VERTEX_ATTR_BLENDWEIGHT		= 1 << 9,
		VERTEX_ATTR_BLENDINDECES	= 1 << 10,
	};

}


//
// Include headers
//
#include "se/Graphics/Window.h"
#include "se/Graphics/GraphicsCore.h"
#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/GraphicsStates.h"
#include "se/Graphics/GPUBuffer.h"
#include "se/Graphics/Shader.h"
