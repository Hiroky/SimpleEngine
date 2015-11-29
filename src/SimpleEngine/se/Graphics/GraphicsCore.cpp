#include "se/Graphics/GraphicsCore.h"
#include "se/Graphics/Window.h"

namespace se
{
	D3D_DRIVER_TYPE			GraphicsCore::driverType_;	
	D3D_FEATURE_LEVEL		GraphicsCore::featureLevel_;
	ID3D11Device*			GraphicsCore::device_;
	IDXGISwapChain*			GraphicsCore::swapChain_;
	ID3D11RenderTargetView* GraphicsCore::renderTargetView_;
	ID3D11Texture2D*        GraphicsCore::depthStencil_;
	ID3D11DepthStencilView* GraphicsCore::depthStencilView_;
	GraphicsContext			GraphicsCore::immediateContext_;


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
		THROW_IF_FAILED(hr)

		// バックバッファ取得
		ID3D11Texture2D* pBackBuffer = NULL;
		hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		THROW_IF_FAILED(hr)

		// レンダーターゲットビューを生成
		hr = device_->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView_);
		THROW_IF_FAILED(hr)
		pBackBuffer->Release();
		pBackBuffer = NULL;


		// デプスステンシルテクスチャ生成
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = device_->CreateTexture2D(&descDepth, NULL, &depthStencil_);
		THROW_IF_FAILED(hr)

		// デプスステンシルビュー
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = device_->CreateDepthStencilView(depthStencil_, &descDSV, &depthStencilView_);
		THROW_IF_FAILED(hr)

		//設定
		deviceContext->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		immediateContext_.Initialize(deviceContext);

		// 仮
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<float>(width);
		vp.Height = static_cast<float>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		deviceContext->RSSetViewports(1, &vp);

		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = TRUE;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.DepthClipEnable = TRUE;
		ID3D11RasterizerState* rs;
		hr = device_->CreateRasterizerState(&rsDesc, &rs);
		THROW_IF_FAILED(hr);
		deviceContext->RSSetState(rs);
		COMPTR_RELEASE(rs);

		// TODO:シェーダ系のマネージャができたらそちらに移す
		VertexLayoutManager::Initialize();
	}


	void GraphicsCore::Finalize()
	{
		VertexLayoutManager::Finalize();

		immediateContext_.Finalize();
		COMPTR_RELEASE(depthStencilView_);
		COMPTR_RELEASE(depthStencil_);
		COMPTR_RELEASE(renderTargetView_);
		COMPTR_RELEASE(swapChain_);
		COMPTR_RELEASE(device_);
	}


	void GraphicsCore::Present(uint syncInterval, uint flags)
	{
		swapChain_->Present(syncInterval, flags);
	}

	void GraphicsCore::SetDefaultRenderTarget()
	{
		immediateContext_.GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);
	}

	void GraphicsCore::ClearRenderTarget()
	{
		float color[4] = { 0.3f, 0.4f, 0.9f, 1.0f };
		immediateContext_.GetDeviceContext()->ClearRenderTargetView(renderTargetView_, color);
		immediateContext_.GetDeviceContext()->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1, 0);
	}

}