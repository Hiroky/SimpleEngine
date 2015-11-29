#pragma once 

#include "se/Common.h"
#include "se/Graphics/GPUBuffer.h"
#include "se/Graphics/Material.h"

namespace se
{
	class MeshResource
	{
	private:
		VertexBuffer* vertexBuffer_;
		IndexBuffer* indexBuffer_;
		Material* material_;

	public:
		MeshResource();
		~MeshResource();
	};
}