#pragma once 

#include <windows.h>
#include <d3d11.h>


namespace se
{
	class GraphicsCore
	{
	private:
		static D3D_DRIVER_TYPE			driverType_;
		static D3D_FEATURE_LEVEL		featureLevel_;
		static ID3D11Device*			device_;
		static ID3D11DeviceContext*		deviceContext_;
		static IDXGISwapChain*			swapChain_;
		static ID3D11RenderTargetView*	renderTargetView_;
		static ID3D11Texture2D*			depthStencil_;
		static ID3D11DepthStencilView*	depthStencilView_;

	public:
		static void Initialize();
		static void Finalize();
		static void Present(uint syncInterval, uint flags);
		static void SetDefaultRenderTarget();

		static ID3D11Device* GetD3DDevice() { return device_; }
		static ID3D11DeviceContext* GetContext() { return deviceContext_; }
		static ID3D11RenderTargetView* GetRenderTarget() { return renderTargetView_; }
		static ID3D11DepthStencilView* GetDepthStencilView() { return depthStencilView_; }
	};
}