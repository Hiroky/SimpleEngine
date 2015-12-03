#pragma once 

#include "se/Common.h"
#include "se/Math/Vector.h"

namespace se
{
	class VertexShader;
	class PixelShader;
	class GPUResource;
	class VertexBuffer;
	class IndexBuffer;
	class ColorBuffer;
	class DepthStencilBuffer;
	class SamplerState;
	class DepthStencilState;
	class RasterizerState;


	//
	// グラフィクスコンテキスト
	//
	class GraphicsContext
	{
	private:
		ID3D11DeviceContext* deviceContext_;

	public:
		GraphicsContext();
		~GraphicsContext();

		void Initialize(ID3D11DeviceContext* context);
		void Finalize();
		ID3D11DeviceContext* GetDeviceContext() { return deviceContext_; }

		// RenderTarget
		void SetRenderTarget(const ColorBuffer* colorBuffers, uint count, const DepthStencilBuffer* depthStencil);
		void ClearRenderTarget(const ColorBuffer& target, const float4& color);
		void ClearDepthStencil(const DepthStencilBuffer& target, float depth = 1.0f);

		// Shader
		void SetVertexShader(const VertexShader& shader);
		void SetPixelShader(const PixelShader& shader);

		// RenderStates
		void SetDepthStencilState(const DepthStencilState& depthStencil);
		void SetRasterizerState(const RasterizerState& raster);

		// Resource binding
		void SetVertexBuffer(int slot, const VertexBuffer* vb);
		void SetIndexBuffer(const IndexBuffer* ib);
		void SetVSResource(uint slot, const GPUResource* resource);
		void SetPSResource(uint slot, const GPUResource* resource);
		void SetPSSamplerState(uint slot, const SamplerState& sampler);

		// Batching
		void DrawIndexed(uint indexStart, uint indexCount);
	};
}