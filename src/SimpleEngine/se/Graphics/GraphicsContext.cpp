#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/Shader.h"
#include "se/Graphics/GPUBuffer.h"
#include "se/Graphics/GraphicsStates.h"

namespace se
{
	GraphicsContext::GraphicsContext()
		: deviceContext_(nullptr)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
		assert(!deviceContext_);
	}

	void GraphicsContext::Initialize(ID3D11DeviceContext * context)
	{
		deviceContext_ = context;
	}

	void GraphicsContext::Finalize()
	{
		if (deviceContext_) {
			deviceContext_->ClearState();
		}
		COMPTR_RELEASE(deviceContext_);
	}

	void GraphicsContext::SetRenderTarget(const ColorBuffer* colorBuffers, uint count, const DepthStencilBuffer* depthStencil)
	{
		ID3D11RenderTargetView* rtvs[8] = { nullptr };
		if (colorBuffers) {
			for (uint i = 0; i < count; i++) {
				rtvs[i] = colorBuffers[i].GetRTV();
			}
		}
		auto* depthStencilView = depthStencil ? depthStencil->GetDSV() : nullptr;
		deviceContext_->OMSetRenderTargets(count, rtvs, depthStencilView);
	}

	void GraphicsContext::ClearRenderTarget(const ColorBuffer& target, const float4& color)
	{
		deviceContext_->ClearRenderTargetView(target.GetRTV(), color.v);
	}

	void GraphicsContext::ClearDepthStencil(const DepthStencilBuffer& target, float depth)
	{
		deviceContext_->ClearDepthStencilView(target.GetDSV(), D3D11_CLEAR_DEPTH, depth, 0);
	}

	void GraphicsContext::SetVertexShader(const VertexShader& shader)
	{
		deviceContext_->VSSetShader(shader.Get(), nullptr, 0);
	}

	void GraphicsContext::SetPixelShader(const PixelShader& shader)
	{
		deviceContext_->PSSetShader(shader.Get(), nullptr, 0);
	}

	void GraphicsContext::SetVertexBuffer(int slot, const VertexBuffer* vb)
	{
		if (!vb) {
			//deviceContext_->IASetVertexBuffers(0, , nullptr, 0);
			return;
		}
		ID3D11Buffer* buffers[] = { vb->Get<ID3D11Buffer>() };
		uint stride = vb->GetStride();
		uint offset = 0;
		deviceContext_->IASetVertexBuffers(slot, 1, buffers, &stride, &offset);

		// レイアウト
		deviceContext_->IASetInputLayout(vb->GetLayout());
	}

	void GraphicsContext::SetIndexBuffer(const IndexBuffer* ib)
	{
		deviceContext_->IASetIndexBuffer(ib->Get<ID3D11Buffer>(), DXGI_FORMAT_R32_UINT, 0);
	}

	void GraphicsContext::SetVSResource(uint slot, const GPUResource* resource)
	{
		ID3D11ShaderResourceView* resources[] = { resource->GetSRV() };
		deviceContext_->VSSetShaderResources(slot, 1, resources);
	}

	void GraphicsContext::SetPSResource(uint slot, const GPUResource* resource)
	{
		ID3D11ShaderResourceView* resources[] = { resource->GetSRV() };
		deviceContext_->PSSetShaderResources(slot, 1, resources);
	}

	void GraphicsContext::SetPSSamplerState(uint slot, const SamplerState& sampler)
	{
		deviceContext_->PSSetSamplers(slot, 1, &sampler.state_);
	}

	void GraphicsContext::DrawIndexed(uint indexStart, uint indexCount)
	{
		deviceContext_->DrawIndexed(indexCount, indexStart, 0);
	}
}