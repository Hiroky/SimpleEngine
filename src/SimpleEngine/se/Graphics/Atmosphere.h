#pragma once 

#include "se/Common.h"
#include "se/Graphics/GraphicsCommon.h"
#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/GPUBuffer.h"

namespace se
{
	class Atmosphere
	{
	private:
		ColorBuffer transmittanceLUT_;	

	public:
		Atmosphere();

		void Initialize();

		void Render(GraphicsContext& context);
		void DebugGUI();
	};
}