#pragma once 

#include "se/Common.h"

namespace se
{
	class VertexShader;
	class PixelShader;
	class GPUResource;
	class VertexBuffer;
	class IndexBuffer;
	class SamplerState;


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

		void SetVertexShader(const VertexShader& shader);
		void SetPixelShader(const PixelShader& shader);
		void SetVertexBuffer(int slot, const VertexBuffer* vb);
		void SetIndexBuffer(const IndexBuffer* ib);

		void SetVSResource(uint slot, const GPUResource* resource);
		void SetPSResource(uint slot, const GPUResource* resource);
		void SetPSSamplerState(uint slot, const SamplerState& sampler);

		void DrawIndexed(uint indexStart, uint indexCount);
	};
}