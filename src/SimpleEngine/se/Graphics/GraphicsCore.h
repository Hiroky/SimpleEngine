#pragma once 

#include <windows.h>
#include <d3d11.h>
#include "se/Graphics/GraphicsContext.h"

namespace se
{
	class GraphicsCore
	{
	private:
		static D3D_DRIVER_TYPE			driverType_;
		static D3D_FEATURE_LEVEL		featureLevel_;
		static ID3D11Device*			device_;
		static IDXGISwapChain*			swapChain_;
		static ID3D11RenderTargetView*	renderTargetView_;
		static ID3D11Texture2D*			depthStencil_;
		static ID3D11DepthStencilView*	depthStencilView_;

		static GraphicsContext			immediateContext_;

	public:
		static void Initialize();
		static void Finalize();
		static void Present(uint syncInterval, uint flags);
		static void SetDefaultRenderTarget();

		static GraphicsContext* GetImmediateContext() { return &immediateContext_; }

		static ID3D11Device* GetDevice() { return device_; }

		// 仮
		static void ClearRenderTarget();
		static ID3D11RenderTargetView* GetRenderTarget() { return renderTargetView_; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return depthStencilView_; }
	};
}