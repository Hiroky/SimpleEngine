#pragma once 

#include "se/Common.h"
#include "se/Graphics/GPUBuffer.h"

namespace se
{
	class MeshResource
	{
	private:
		VertexBuffer vertexBuffer_;
		IndexBuffer indexBuffer_;

	public:
		MeshResource();
		~MeshResource();
	};
}