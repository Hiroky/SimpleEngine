#include "se/Graphics/GraphicsCore.h"
#include "se/Graphics/Window.h"
#include "se/Graphics/Shader.h"
#include "se/Graphics/GraphicsStates.h"

namespace se
{
	D3D_DRIVER_TYPE			GraphicsCore::driverType_;	
	D3D_FEATURE_LEVEL		GraphicsCore::featureLevel_;
	ID3D11Device*			GraphicsCore::device_;
	IDXGISwapChain*			GraphicsCore::swapChain_;
	GraphicsContext			GraphicsCore::immediateContext_;
	ColorBuffer				GraphicsCore::displayBuffer_;
	DepthStencilBuffer		GraphicsCore::displayDepthBuffer_;

	void GraphicsCore::Initialize()
	{
		HRESULT hr = S_OK;
		HWND hWnd = se::Window::GetHWND();

		// ウィンドウサイズを取得
		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// ドライバータイプ
		D3D_DRIVER_TYPE driverTypes[] = {
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

		// 機能レベル
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
		};
		UINT numFeatureLevels = sizeof(featureLevels) / sizeof(featureLevels[0]);

		// スワップチェインの設定
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		ID3D11DeviceContext* deviceContext;

		// デバイスとスワップチェインを作成する
		for (UINT idx = 0; idx < numDriverTypes; idx++) {
			driverType_ = driverTypes[idx];
			hr = D3D11CreateDeviceAndSwapChain(
				NULL,
				driverType_,
				NULL,
				createDeviceFlags,
				featureLevels,
				numFeatureLevels,
				D3D11_SDK_VERSION,
				&sd,
				&swapChain_,
				&device_,
				&featureLevel_,
				&deviceContext);

			if (SUCCEEDED(hr)) {
				// 成功したらループ脱出
				break;
			}
		}
		THROW_IF_FAILED(hr);

		// バックバッファ取得
		ID3D11Texture2D* pBackBuffer = NULL;
		hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		THROW_IF_FAILED(hr);

		// レンダーターゲットビューを生成
		ID3D11RenderTargetView* renderTargetView;
		hr = device_->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
		THROW_IF_FAILED(hr)
		pBackBuffer->Release();
		pBackBuffer = NULL;
		displayBuffer_.InitializeDisplayBuffer(renderTargetView);

		// デプスステンシルテクスチャ生成
		displayDepthBuffer_.Create(width, height);

		//設定
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// コンテキスト
		immediateContext_.Initialize(deviceContext);

		// ステート
		SamplerState::Initialize();
		DepthStencilState::Initialize();
		RasterizerState::Initialize();
		immediateContext_.SetDepthStencilState(DepthStencilState::Get(DepthStencilState::Disable));
		immediateContext_.SetRasterizerState(RasterizerState::Get(RasterizerState::BackFaceCull));

		// 仮
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<float>(width);
		vp.Height = static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		deviceContext->RSSetViewports(1, &vp);
		D3D11_RECT rect;
		rect.top = rect.left = 0;
		rect.right = width;
		rect.bottom = height;
		deviceContext->RSSetScissorRects(1, &rect);

		VertexLayoutManager::Get().Initialize();
	}


	void GraphicsCore::Finalize()
	{
		VertexLayoutManager::Get().Finalize();

		immediateContext_.Finalize();
		COMPTR_RELEASE(swapChain_);
		COMPTR_RELEASE(device_);
	}


	void GraphicsCore::Present(uint32_t syncInterval, uint32_t flags)
	{
		swapChain_->Present(syncInterval, flags);
	}

}