#pragma once 

#include "se/Common.h"

namespace se
{
	class VertexShader;
	class PixelShader;
	class GPUBuffer;
	class VertexBuffer;
	class IndexBuffer;


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
		ID3D11DeviceContext* GetDeviceContext() { return deviceContext_; }

		void SetVertexShader(const VertexShader& shader);
		void SetPixelShader(const PixelShader& shader);
		void SetVertexBuffer(int slot, const VertexBuffer* vb);
		void SetIndexBuffer(const IndexBuffer* ib);

		void DrawIndexed(uint indexStart, uint indexCount);
	};
}