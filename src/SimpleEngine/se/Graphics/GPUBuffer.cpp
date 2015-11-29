#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/GPUBuffer.h"
#include "se/Graphics/DDSTextureLoader.h"


namespace se
{
#pragma region GPUResource

	GPUResource::GPUResource()
		: resource_(nullptr)
		, srv_(nullptr)
		, uav_(nullptr)
	{
	}

	GPUResource::~GPUResource()
	{
		Destroy();
	}

	void GPUResource::Destroy()
	{
		COMPTR_RELEASE(srv_);
		COMPTR_RELEASE(uav_);
		COMPTR_RELEASE(resource_);
	}

#pragma endregion

#pragma region VertexBuffer

	VertexBuffer::VertexBuffer()
		: layout_(nullptr)
		, stride_(0)
		, attributes_(0)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		COMPTR_RELEASE(layout_);
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
		GPUResource::Destroy();
	}

#pragma endregion

#pragma region IndexBuffer

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
		GPUResource::Destroy();
	}

#pragma endregion

#pragma region PixelBuffer

	PixelBuffer::PixelBuffer()
		: width_(0)
		, height_(0)
		, depth_(0)
		, format_(0)
	{
	}
	PixelBuffer::~PixelBuffer()
	{
	}

#pragma endregion

#pragma region ColorBuffer

	ColorBuffer::ColorBuffer()
	{
	}
	ColorBuffer::~ColorBuffer()
	{
	}

#pragma endregion

#pragma region DepthStencilBuffer

	DepthStencilBuffer::DepthStencilBuffer()
	{
	}

	DepthStencilBuffer::~DepthStencilBuffer()
	{
	}

#pragma endregion

#pragma region Texture

	Texture::Texture()
	{
	}
	Texture::~Texture()
	{
	}

	void Texture::LoadFromFile(const char* fileName)
	{
		wchar_t buffer[255] = { 0 };
		size_t size;
		mbstowcs_s(&size, buffer, fileName, sizeof(buffer));
		DirectX::CreateDDSTextureFromFile(GraphicsCore::GetDevice(), buffer, &resource_, &srv_);
	}

	void Texture::LoadFromMemory(const void * data, uint size)
	{
		DirectX::CreateDDSTextureFromMemory(GraphicsCore::GetDevice(), reinterpret_cast<const byte*>(data), static_cast<size_t>(size), &resource_, &srv_);
	}

#pragma endregion
}