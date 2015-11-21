#pragma once 

#include "se/Common.h"

namespace se
{
	class GraphicsContext
	{
	private:
		ID3D11DeviceContext*		deviceContext_;

	public:
		GraphicsContext(ID3D11DeviceContext* context);
		~GraphicsContext();

		ID3D11DeviceContext* GetDeviceContext() { return deviceContext_; }
	};
}