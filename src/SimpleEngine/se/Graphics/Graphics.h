#pragma once

#include "se/Graphics/Window.h"
#include "se/Graphics/GraphicsCore.h"
#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/GPUBuffer.h"

#ifndef COMPTR_RELEASE
	#define COMPTR_RELEASE(p)		if(p) { p->Release(); p = nullptr; }
#endif

#ifndef THROW_IF_FAILED
	#define THROW_IF_FAILED(hr) if (FAILED(hr)) { throw; }
#endif
