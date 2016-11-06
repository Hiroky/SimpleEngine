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

	void GraphicsContext::SetRenderTarget(const ColorBuffer* colorBuffers, uint32_t count, const DepthStencilBuffer* depthStencil)
	{
		ID3D11RenderTargetView* rtvs[8] = { nullptr };
		if (colorBuffers) {
			for (uint32_t i = 0; i < count; i++) {
				rtvs[i] = colorBuffers[i].GetRTV();
			}
		}
		auto* depthStencilView = depthStencil ? depthStencil->GetDSV() : nullptr;
		deviceContext_->OMSetRenderTargets(count, rtvs, depthStencilView);
	}

	void GraphicsContext::ClearRenderTarget(const ColorBuffer& target, const float4& color)
	{
		deviceContext_->ClearRenderTargetView(target.GetRTV(), color.ToFloatArray());
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

	void GraphicsContext::SetComputeShader(const ComputeShader& shader)
	{
		deviceContext_->CSSetShader(shader.Get(), nullptr, 0);
	}

	void GraphicsContext::SetBlendState(const BlendState& blend)
	{
		deviceContext_->OMSetBlendState(blend.state_, 0, 0xFFFFFFFF);
	}

	void GraphicsContext::SetDepthStencilState(const DepthStencilState& depthStencil)
	{
		deviceContext_->OMSetDepthStencilState(depthStencil.state_, 0);
	}

	void GraphicsContext::SetRasterizerState(const RasterizerState& raster)
	{
		deviceContext_->RSSetState(raster.state_);
	}

	void GraphicsContext::SetPrimitiveType(PrimitiveType type)
	{
		static const D3D_PRIMITIVE_TOPOLOGY types[] = {
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		};
		deviceContext_->IASetPrimitiveTopology(types[type]);
	}

	void GraphicsContext::SetViewport(const Rect& rect, float minDepth, float maxDepth)
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = static_cast<float>(rect.x);
		vp.TopLeftY = static_cast<float>(rect.y);
		vp.Width = static_cast<float>(rect.width);
		vp.Height = static_cast<float>(rect.height);
		vp.MinDepth = minDepth;
		vp.MaxDepth = maxDepth;
		deviceContext_->RSSetViewports(1, &vp);
	}

	void GraphicsContext::SetScissorRect(const Rect& rect)
	{
		D3D11_RECT r = { rect.x, rect.y, rect.width, rect.height };
		deviceContext_->RSSetScissorRects(1, &r);
	}

	void GraphicsContext::SetViewportAndScissorRect(const Rect& rect, float minDepth, float maxDepth)
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = static_cast<float>(rect.x);
		vp.TopLeftY = static_cast<float>(rect.y);
		vp.Width = static_cast<float>(rect.width);
		vp.Height = static_cast<float>(rect.height);
		vp.MinDepth = minDepth;
		vp.MaxDepth = maxDepth;
		deviceContext_->RSSetViewports(1, &vp);
		D3D11_RECT r = { rect.x, rect.y, rect.width, rect.height };
		deviceContext_->RSSetScissorRects(1, &r);
	}

	void GraphicsContext::SetInputLayout(const VertexInputLayout& layout)
	{
		deviceContext_->IASetInputLayout(layout.layout);
	}

	void GraphicsContext::SetVertexBuffer(int slot, const VertexBuffer* vb)
	{
		ID3D11Buffer* buffers[] = { vb->Get<ID3D11Buffer>() };
		uint32_t stride = vb->GetStride();
		uint32_t offset = 0;
		deviceContext_->IASetVertexBuffers(slot, 1, buffers, &stride, &offset);
	}

	void GraphicsContext::SetIndexBuffer(const IndexBuffer* ib)
	{
		static DXGI_FORMAT indexFormat[] = { DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R32_UINT };
		deviceContext_->IASetIndexBuffer(ib->Get<ID3D11Buffer>(), indexFormat[ib->GetStride()], 0);
	}

	void GraphicsContext::SetVSResource(uint32_t slot, const GPUResource* resource)
	{
		ID3D11ShaderResourceView* resources[] = { resource->GetSRV() };
		deviceContext_->VSSetShaderResources(slot, 1, resources);
	}

	void GraphicsContext::SetPSResource(uint32_t slot, const GPUResource* resource)
	{
		ID3D11ShaderResourceView* resources[] = { resource->GetSRV() };
		deviceContext_->PSSetShaderResources(slot, 1, resources);
	}

	void GraphicsContext::SetPSSamplerState(uint32_t slot, const SamplerState& sampler)
	{
		deviceContext_->PSSetSamplers(slot, 1, &sampler.state_);
	}

	void GraphicsContext::SetCSResource(uint32_t slot, const GPUResource* resource)
	{
		ID3D11ShaderResourceView* resources[] = { resource->GetSRV() };
		deviceContext_->CSSetShaderResources(slot, 1, resources);
	}

	void GraphicsContext::SetCSSamplerState(uint32_t slot, const SamplerState& sampler)
	{
		deviceContext_->CSSetSamplers(slot, 1, &sampler.state_);
	}

	void GraphicsContext::SetCSUnorderedAccessView(uint32_t slot, const GPUResource* resource)
	{
		ID3D11UnorderedAccessView* resources[] = { resource->GetUAV() };
		deviceContext_->CSSetUnorderedAccessViews(slot, 1, resources, nullptr);
	}

	void GraphicsContext::SetVSConstantBuffer(uint32_t slot, const ConstantBuffer& buffer)
	{
		deviceContext_->VSSetConstantBuffers(slot, 1, &buffer.buffer_);
	}

	void GraphicsContext::SetPSConstantBuffer(uint32_t slot, const ConstantBuffer& buffer)
	{
		deviceContext_->PSSetConstantBuffers(slot, 1, &buffer.buffer_);
	}

	void GraphicsContext::SetCSConstantBuffer(uint32_t slot, const ConstantBuffer& buffer)
	{
		deviceContext_->CSSetConstantBuffers(slot, 1, &buffer.buffer_);
	}

	void GraphicsContext::DrawIndexed(uint32_t indexStart, uint32_t indexCount, uint32_t vertexStart)
	{
		deviceContext_->DrawIndexed(indexCount, indexStart, vertexStart);
	}

	void GraphicsContext::Dispatch(uint32_t x, uint32_t y, uint32_t z)
	{
		deviceContext_->Dispatch(x, y, z);
	}

	void * GraphicsContext::Map(GPUResource& resource, uint32_t subResource)
	{
		D3D11_MAPPED_SUBRESOURCE mapResource;
		THROW_IF_FAILED(deviceContext_->Map(resource.GetResource(), subResource, D3D11_MAP_WRITE_DISCARD, 0 , &mapResource));
		return mapResource.pData;
	}

	void GraphicsContext::Unmap(GPUResource& resource, uint32_t subResource)
	{
		deviceContext_->Unmap(resource.GetResource(), subResource);
	}

	void GraphicsContext::UpdateSubresource(ConstantBuffer& resource, const void* data, size_t size)
	{
		deviceContext_->UpdateSubresource(resource.buffer_, 0, nullptr, data, 0, 0);
	}

	void GraphicsContext::BeginQuery(Query& query)
	{
		deviceContext_->Begin(query.query_);
	}

	void GraphicsContext::EndQuery(Query& query)
	{
		deviceContext_->End(query.query_);
	}

	void GraphicsContext::GetQueryData(Query& query, void* data, uint32_t size)
	{
		deviceContext_->GetData(query.query_, data, size, 0);
	}

}