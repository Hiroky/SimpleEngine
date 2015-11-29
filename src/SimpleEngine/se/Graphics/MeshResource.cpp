#include "se/Graphics/GraphicsContext.h"
#include "MeshResource.h"


namespace se
{
	MeshResource::MeshResource()
		: vertexBuffer_(nullptr)
		, indexBuffer_(nullptr)
		, material_(nullptr)
	{
	}

	MeshResource::~MeshResource()
	{
	}
}