#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics//Shader.h"

namespace se
{
	GraphicsContext::GraphicsContext()
		: deviceContext_(nullptr)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
		if (deviceContext_) {
			deviceContext_->ClearState();
		}
		COMPTR_RELEASE(deviceContext_);
	}

	void GraphicsContext::Initialize(ID3D11DeviceContext * context)
	{
		deviceContext_ = context;
	}

	void GraphicsContext::SetVertexShader(const VertexShader& shader)
	{
		deviceContext_->VSSetShader(shader.Get(), nullptr, 0);

		// ひとまずここでレイアウトもセットしておく
		deviceContext_->IASetInputLayout(shader.inputLayout_);
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
	}

	void GraphicsContext::SetIndexBuffer(const IndexBuffer* ib)
	{
		deviceContext_->IASetIndexBuffer(ib->Get<ID3D11Buffer>(), DXGI_FORMAT_R32_UINT, 0);
	}

	void GraphicsContext::DrawIndexed(uint indexStart, uint indexCount)
	{
		deviceContext_->DrawIndexed(indexCount, indexStart, 0);
	}
}