#include "se/Graphics/GraphicsCore.h"
#include "se/Graphics/Window.h"
#include "se/Graphics/Shader.h"
#include "se/Graphics/GraphicsStates.h"
#include "se/Graphics/GPUProfiler.h"

namespace se
{
	D3D_DRIVER_TYPE			GraphicsCore::driverType_;	
	D3D_FEATURE_LEVEL		GraphicsCore::featureLevel_;
	ID3D11Device*			GraphicsCore::device_;
	IDXGISwapChain*			GraphicsCore::swapChain_;
	GraphicsContext			GraphicsCore::immediateContext_;
	ColorBuffer				GraphicsCore::displayBuffer_;
	DepthStencilBuffer		GraphicsCore::displayDepthBuffer_;
	uint32_t				GraphicsCore::width_;
	uint32_t				GraphicsCore::height_;

	void GraphicsCore::Initialize()
	{
		HRESULT hr = S_OK;
		HWND hWnd = se::Window::GetHWND();

		// ウィンドウサイズを取得
		RECT rc;
		GetClientRect(hWnd, &rc);
		width_ = rc.right - rc.left;
		height_ = rc.bottom - rc.top;

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
		sd.BufferDesc.Width = width_;
		sd.BufferDesc.Height = height_;
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
		displayDepthBuffer_.Create(width_, height_);

		// コンテキスト
		immediateContext_.Initialize(deviceContext);

		// ステート
		SamplerState::Initialize();
		BlendState::Initialize();
		DepthStencilState::Initialize();
		RasterizerState::Initialize();
		immediateContext_.SetRasterizerState(RasterizerState::Get(RasterizerState::BackFaceCull));
		immediateContext_.SetPrimitiveType(PRIMITIVE_TYPE_TRIANGLE_LIST);
		immediateContext_.SetViewportAndScissorRect(Rect(0, 0, width_, height_));

		// 頂点レイアウトマネージャ
		VertexLayoutManager::Get().Initialize();

		// プロファイラ
		GPUProfiler::Get().Initialize();
	}


	void GraphicsCore::Finalize()
	{
		VertexLayoutManager::Get().Finalize();
		GPUProfiler::Get().Finalize();

		immediateContext_.Finalize();
		COMPTR_RELEASE(swapChain_);
		COMPTR_RELEASE(device_);
	}


	void GraphicsCore::Present(uint32_t syncInterval, uint32_t flags)
	{
		swapChain_->Present(syncInterval, flags);
	}

}