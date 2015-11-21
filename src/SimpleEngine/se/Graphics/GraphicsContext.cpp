#include "se/Graphics/GraphicsContext.h"


namespace se
{
	GraphicsContext::GraphicsContext(ID3D11DeviceContext * context)
		: deviceContext_(context)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
		COMPTR_RELEASE(deviceContext_);
	}
}