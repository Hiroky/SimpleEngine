#pragma once 

#include "se/Common.h"

namespace se
{
	class GPUBuffer
	{
	private:
		ID3D11Resource*		resource_;

	public:
		GPUBuffer();
		~GPUBuffer();

	};
}