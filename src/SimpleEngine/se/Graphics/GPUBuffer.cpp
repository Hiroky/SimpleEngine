#include "se/Graphics/GraphicsContext.h"
#include "GPUBuffer.h"


namespace se
{
	GPUBuffer::GPUBuffer()
		: resource_(nullptr)
	{
	}

	GPUBuffer::~GPUBuffer()
	{
		Destroy();
	}

	void GPUBuffer::Destroy()
	{
		COMPTR_RELEASE(resource_);
	}



	VertexBuffer::VertexBuffer()
		: stride_(0)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	void VertexBuffer::CreateBuffer(const VertexBufferDesc& desc)
	{
		// 頂点バッファの設定
		// DYNAMICに対応する
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.CPUAccessFlags = 0;
		bd.ByteWidth = desc.size;

		if (desc.canUnorderedAccess) {
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			bd.Usage = D3D11_USAGE_DEFAULT;
		} else {
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_IMMUTABLE;
		}

		// サブリソースの設定
		D3D11_SUBRESOURCE_DATA* pInit = nullptr;
		D3D11_SUBRESOURCE_DATA initData;
		if (desc.data) {
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = desc.data;
			pInit = &initData;
		}

		// 頂点バッファ生成
		ID3D11Buffer* buffer;
		HRESULT hr = GraphicsCore::GetDevice()->CreateBuffer(&bd, pInit, &buffer);
		THROW_IF_FAILED(hr);
		resource_ = buffer;
		stride_ = desc.stride;

		// アンオーダードアクセスビューを生成
		if (desc.canUnorderedAccess) {
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = 0;
			uavDesc.Buffer.NumElements = desc.size / desc.stride;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Format = DXGI_FORMAT_R32_UINT;
			//hr = GraphicsCore::GetDevice()->CreateUnorderedAccessView(resource_, &uavDesc, &m_pUnorderedAccessView);
			THROW_IF_FAILED(hr);
		}

	}

	void VertexBuffer::DestroyBuffer()
	{
		GPUBuffer::Destroy();
	}



	IndexBuffer::IndexBuffer()
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	void IndexBuffer::CreateBuffer(const void * data, uint size, IndexBufferStride stride)
	{
		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = size;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;

		ID3D11Buffer* buffer;
		HRESULT hr = GraphicsCore::GetDevice()->CreateBuffer(&ibd, &initData, &buffer);
		THROW_IF_FAILED(hr);
		resource_ = buffer;
	}

	void IndexBuffer::DestroyBuffer()
	{
		GPUBuffer::Destroy();
	}
}