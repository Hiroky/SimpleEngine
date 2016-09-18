#include "se/Graphics/GPUBuffer.h"
#include "se/Graphics/GraphicsCore.h"
#include "se/Graphics/GraphicsContext.h"
#include "se/Graphics/Shader.h"
#include "se/Graphics/DDSTextureLoader.h"


namespace se
{
	namespace {

		// Format -> DXGI_FORMAT
		DXGI_FORMAT GetDXGIFormat(Format format)
		{
			static const DXGI_FORMAT nativeFormats[] = {
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R32G32B32A32_TYPELESS,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				DXGI_FORMAT_R32G32B32A32_UINT,
				DXGI_FORMAT_R32G32B32A32_SINT,
				DXGI_FORMAT_R32G32B32_TYPELESS,
				DXGI_FORMAT_R32G32B32_FLOAT,
				DXGI_FORMAT_R32G32B32_UINT,
				DXGI_FORMAT_R32G32B32_SINT,
				DXGI_FORMAT_R16G16B16A16_TYPELESS,
				DXGI_FORMAT_R16G16B16A16_FLOAT,
				DXGI_FORMAT_R16G16B16A16_UNORM,
				DXGI_FORMAT_R16G16B16A16_UINT,
				DXGI_FORMAT_R16G16B16A16_SNORM,
				DXGI_FORMAT_R16G16B16A16_SINT,
				DXGI_FORMAT_R32G32_TYPELESS,
				DXGI_FORMAT_R32G32_FLOAT,
				DXGI_FORMAT_R32G32_UINT,
				DXGI_FORMAT_R32G32_SINT,
				DXGI_FORMAT_R32G8X24_TYPELESS,
				DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
				DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
				DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
				DXGI_FORMAT_R10G10B10A2_TYPELESS,
				DXGI_FORMAT_R10G10B10A2_UNORM,
				DXGI_FORMAT_R10G10B10A2_UINT,
				DXGI_FORMAT_R11G11B10_FLOAT,
				DXGI_FORMAT_R8G8B8A8_TYPELESS,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_R8G8B8A8_UINT,
				DXGI_FORMAT_R8G8B8A8_SNORM,
				DXGI_FORMAT_R8G8B8A8_SINT,
				DXGI_FORMAT_R16G16_TYPELESS,
				DXGI_FORMAT_R16G16_FLOAT,
				DXGI_FORMAT_R16G16_UNORM,
				DXGI_FORMAT_R16G16_UINT,
				DXGI_FORMAT_R16G16_SNORM,
				DXGI_FORMAT_R16G16_SINT,
				DXGI_FORMAT_R32_TYPELESS,
				DXGI_FORMAT_D32_FLOAT,
				DXGI_FORMAT_R32_FLOAT,
				DXGI_FORMAT_R32_UINT,
				DXGI_FORMAT_R32_SINT,
				DXGI_FORMAT_R24G8_TYPELESS,
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
				DXGI_FORMAT_X24_TYPELESS_G8_UINT,
				DXGI_FORMAT_R8G8_TYPELESS,
				DXGI_FORMAT_R8G8_UNORM,
				DXGI_FORMAT_R8G8_UINT,
				DXGI_FORMAT_R8G8_SNORM,
				DXGI_FORMAT_R8G8_SINT,
				DXGI_FORMAT_R16_TYPELESS,
				DXGI_FORMAT_R16_FLOAT,
				DXGI_FORMAT_D16_UNORM,
				DXGI_FORMAT_R16_UNORM,
				DXGI_FORMAT_R16_UINT,
				DXGI_FORMAT_R16_SNORM,
				DXGI_FORMAT_R16_SINT,
				DXGI_FORMAT_R8_TYPELESS,
				DXGI_FORMAT_R8_UNORM,
				DXGI_FORMAT_R8_UINT,
				DXGI_FORMAT_R8_SNORM,
				DXGI_FORMAT_R8_SINT,
				DXGI_FORMAT_A8_UNORM,
				DXGI_FORMAT_R1_UNORM,
				DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
				DXGI_FORMAT_R8G8_B8G8_UNORM,
				DXGI_FORMAT_G8R8_G8B8_UNORM,
				DXGI_FORMAT_BC1_TYPELESS,
				DXGI_FORMAT_BC1_UNORM,
				DXGI_FORMAT_BC1_UNORM_SRGB,
				DXGI_FORMAT_BC2_TYPELESS,
				DXGI_FORMAT_BC2_UNORM,
				DXGI_FORMAT_BC2_UNORM_SRGB,
				DXGI_FORMAT_BC3_TYPELESS,
				DXGI_FORMAT_BC3_UNORM,
				DXGI_FORMAT_BC3_UNORM_SRGB,
				DXGI_FORMAT_BC4_TYPELESS,
				DXGI_FORMAT_BC4_UNORM,
				DXGI_FORMAT_BC4_SNORM,
				DXGI_FORMAT_BC5_TYPELESS,
				DXGI_FORMAT_BC5_UNORM,
				DXGI_FORMAT_BC5_SNORM,
				DXGI_FORMAT_B5G6R5_UNORM,
				DXGI_FORMAT_B5G5R5A1_UNORM,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				DXGI_FORMAT_B8G8R8X8_UNORM,
				DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
				DXGI_FORMAT_B8G8R8A8_TYPELESS,
				DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
				DXGI_FORMAT_B8G8R8X8_TYPELESS,
				DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
				DXGI_FORMAT_BC6H_TYPELESS,
				DXGI_FORMAT_BC6H_UF16,
				DXGI_FORMAT_BC6H_SF16,
				DXGI_FORMAT_BC7_TYPELESS,
				DXGI_FORMAT_BC7_UNORM,
				DXGI_FORMAT_BC7_UNORM_SRGB,
			};

			return nativeFormats[format];
		};

	}

#pragma region ConstantBuffer

	ConstantBuffer::ConstantBuffer()
		: buffer_(nullptr)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		COMPTR_RELEASE(buffer_);
	}

	void ConstantBuffer::Create(uint32_t size, BufferUsage usage)
	{
		Assert(size % 16 == 0);
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = size;
		switch (usage)
		{
		case BUFFER_USAGE_DYNAMIC:
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		default:
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
			break;
		}

		THROW_IF_FAILED(GraphicsCore::GetDevice()->CreateBuffer(&bd, nullptr, &buffer_));
		size_ = size;
	}

	void ConstantBuffer::Update(GraphicsContext& context, const void* data, uint32_t size)
	{
		context.UpdateSubresource(*this, data, size);
	}

#pragma endregion

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

	namespace
	{
		uint32_t ComputeVertexStride(uint32_t attr)
		{
			uint32_t size = 0;
			if (attr & VERTEX_ATTR_FLAG_POSITION) size += 12;
			if (attr & VERTEX_ATTR_FLAG_NORMAL) size += 12;
			if (attr & VERTEX_ATTR_FLAG_COLOR) size += 16;
			if (attr & VERTEX_ATTR_FLAG_BYTE_COLOR) size += 4;
			if (attr & VERTEX_ATTR_FLAG_TEXCOORD0) size += 8;
			if (attr & VERTEX_ATTR_FLAG_TEXCOORD1) size += 8;
			if (attr & VERTEX_ATTR_FLAG_TEXCOORD2) size += 8;
			if (attr & VERTEX_ATTR_FLAG_TEXCOORD3) size += 8;
			if (attr & VERTEX_ATTR_FLAG_TANGENT) size += 12;
			if (attr & VERTEX_ATTR_FLAG_BITANGENT) size += 12;
			return size;
		}
	}

	VertexBuffer::VertexBuffer()
		: stride_(0)
		, attributes_(0)
		, size_(0)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	void VertexBuffer::Create(const void* data, uint32_t size, VertexAttributeFlags attributes, BufferUsage usage, bool unorderedAccess)
	{
		Assert(!resource_);

		size_ = size;

		// 頂点バッファの設定
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.CPUAccessFlags = 0;
		bd.ByteWidth = size;

		if (unorderedAccess) {
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		}
		else {
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			if (usage == BUFFER_USAGE_DYNAMIC) {
				bd.Usage = D3D11_USAGE_DYNAMIC;
				bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			} else if (usage == BUFFER_USAGE_DEFAULT) {
				bd.Usage = D3D11_USAGE_DEFAULT;
			} else {
				bd.Usage = D3D11_USAGE_IMMUTABLE;
			}
		}

		// サブリソースの設定
		D3D11_SUBRESOURCE_DATA* pInit = nullptr;
		D3D11_SUBRESOURCE_DATA initData;
		if (data) {
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = data;
			pInit = &initData;
		}

		// 頂点バッファ生成
		ID3D11Buffer* buffer;
		THROW_IF_FAILED(GraphicsCore::GetDevice()->CreateBuffer(&bd, pInit, &buffer));

		resource_ = buffer;
		stride_ = ComputeVertexStride(attributes);
		attributes_ = attributes;

		// アンオーダードアクセスビューを生成
		if (unorderedAccess) {
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			uavDesc.Buffer.NumElements = size / 4;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			THROW_IF_FAILED(GraphicsCore::GetDevice()->CreateUnorderedAccessView(resource_, &uavDesc, &uav_));
		}
	}

	void VertexBuffer::Create(uint32_t vertexNum, VertexAttributeFlags attributes, BufferUsage usage, bool unorderedAccess)
	{
		Create(nullptr, vertexNum * ComputeVertexStride(attributes), attributes, usage, unorderedAccess);
	}

	void VertexBuffer::Destroy()
	{
		GPUResource::Destroy();
	}

#pragma endregion

#pragma region IndexBuffer

	namespace {
		static uint32_t indexStrideList[] = { 2, 4 };
	}

	IndexBuffer::IndexBuffer()
		: stride_(INDEX_BUFFER_STRIDE_U16)
		, bufferSize_(0)
		, indexCount_(0)

	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	void IndexBuffer::Create(const void* data, uint32_t size, IndexBufferStride stride, BufferUsage usage)
	{
		stride_ = stride;
		bufferSize_ = size;
		indexCount_ = indexStrideList[stride];

		D3D11_BUFFER_DESC ibd;
		ibd.ByteWidth = size;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		if (usage == BUFFER_USAGE_DYNAMIC) {
			ibd.Usage = D3D11_USAGE_DYNAMIC;
			ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		} else if (usage == BUFFER_USAGE_DEFAULT) {
			ibd.Usage = D3D11_USAGE_DEFAULT;
		} else {
			ibd.Usage = D3D11_USAGE_IMMUTABLE;
		}

		D3D11_SUBRESOURCE_DATA* pInit = nullptr;
		D3D11_SUBRESOURCE_DATA initData;
		if (data) {
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = data;
			pInit = &initData;
		}

		ID3D11Buffer* buffer;
		HRESULT hr = GraphicsCore::GetDevice()->CreateBuffer(&ibd, pInit, &buffer);
		THROW_IF_FAILED(hr);
		resource_ = buffer;
	}

	void IndexBuffer::Create(uint32_t num, BufferUsage usage)
	{
		IndexBufferStride stride = (num > USHRT_MAX) ? INDEX_BUFFER_STRIDE_U32 : INDEX_BUFFER_STRIDE_U16;
		Create(nullptr, num * indexStrideList[stride], stride, usage);
	}

	void IndexBuffer::Destroy()
	{
		GPUResource::Destroy();
	}

#pragma endregion

#pragma region PixelBuffer

	PixelBuffer::PixelBuffer()
		: width_(0)
		, height_(0)
		, depth_(0)
		, format_(FORMAT_UNKNOWN)
	{
	}
	PixelBuffer::~PixelBuffer()
	{
	}

#pragma endregion

#pragma region ColorBuffer

	ColorBuffer::ColorBuffer()
		: rtv_(nullptr)
	{
	}

	ColorBuffer::~ColorBuffer()
	{
		COMPTR_RELEASE(rtv_);
	}

	void ColorBuffer::InitializeDisplayBuffer(ID3D11RenderTargetView* renderTarget)
	{
		rtv_ = renderTarget;
		rtv_->GetResource(&resource_);
		ID3D11Texture2D* texture = (ID3D11Texture2D*)resource_;
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);
		width_ = desc.Width;
		height_ = desc.Height;
		depth_ = desc.ArraySize;
	}

	void ColorBuffer::Create2D(Format format, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mips)
	{
		Assert(!resource_);
		mips = se::Max<uint32_t>(1, mips);
		width_ = width;
		height_ = height;
		depth_ = arraySize;
		format_ = format;

		DXGI_FORMAT dxgiFormat = GetDXGIFormat(format);

		// テクスチャ生成
		D3D11_TEXTURE2D_DESC objdesc;
		ZeroMemory(&objdesc, sizeof(objdesc));
		objdesc.Width = width;
		objdesc.Height = height;
		objdesc.MipLevels = mips;
		objdesc.ArraySize = arraySize;
		objdesc.SampleDesc.Count = 1;
		objdesc.SampleDesc.Quality = 0;
		objdesc.MiscFlags = 0;
		objdesc.Format = dxgiFormat;
		objdesc.Usage = D3D11_USAGE_DEFAULT;
		objdesc.CPUAccessFlags = 0;
		objdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		//	テクスチャ生成
		auto* device = GraphicsCore::GetDevice();
		ID3D11Texture2D* texture;
		THROW_IF_FAILED(device->CreateTexture2D(&objdesc, nullptr, &texture));
		resource_ = texture;

		//	シェーダリソースビュー
		bool isArray = (arraySize > 1);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = dxgiFormat;
		if (isArray) {
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.MipLevels = mips;
		}
		else {
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = mips;
		}
		THROW_IF_FAILED(device->CreateShaderResourceView(texture, &srvDesc, &srv_));

		// レンダーターゲットビュー
		D3D11_RENDER_TARGET_VIEW_DESC rdesc;
		rdesc.Format = dxgiFormat;
		rdesc.ViewDimension = (arraySize > 1 || mips > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DARRAY : D3D11_RTV_DIMENSION_TEXTURE2D;
		rdesc.Texture2DArray.ArraySize = 1;
		rdesc.Texture2DArray.MipSlice = 0;
		rdesc.Texture2DArray.FirstArraySlice = 0;
		THROW_IF_FAILED(device->CreateRenderTargetView(texture, &rdesc, &rtv_));
	}

	void ColorBuffer::Destroy()
	{
		COMPTR_RELEASE(rtv_);
		PixelBuffer::Destroy();
	}

#pragma endregion

#pragma region DepthStencilBuffer

	DepthStencilBuffer::DepthStencilBuffer()
	{
	}

	DepthStencilBuffer::~DepthStencilBuffer()
	{
		COMPTR_RELEASE(dsv_);
	}

	void DepthStencilBuffer::Create(uint32_t width, uint32_t height)
	{
		auto* device = GraphicsCore::GetDevice();

		// デプスステンシルテクスチャ
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
		ID3D11Texture2D* texture2d;
		auto hr = device->CreateTexture2D(&descDepth, NULL, &texture2d);
		THROW_IF_FAILED(hr);
		resource_ = texture2d;

		// デプスステンシルビュー
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(texture2d, &descDSV, &dsv_);
		THROW_IF_FAILED(hr);
	}

	void DepthStencilBuffer::Destroy()
	{
		COMPTR_RELEASE(dsv_);
		PixelBuffer::Destroy();
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
		auto hr = DirectX::CreateDDSTextureFromFile(GraphicsCore::GetDevice(), buffer, &resource_, &srv_);
		THROW_IF_FAILED(hr);

		// 2D only
		ID3D11Texture2D* texture = (ID3D11Texture2D*)resource_;
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);
		width_ = desc.Width;
		height_ = desc.Height;
		depth_ = desc.ArraySize;
	}

	void Texture::LoadFromMemory(const void* data, uint32_t size)
	{
		DirectX::CreateDDSTextureFromMemory(GraphicsCore::GetDevice(), reinterpret_cast<const byte*>(data), static_cast<size_t>(size), &resource_, &srv_);
		
		// 2D only
		ID3D11Texture2D* texture = (ID3D11Texture2D*)resource_;
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);
		width_ = desc.Width;
		height_ = desc.Height;
		depth_ = desc.ArraySize;
	}

#pragma endregion

#pragma region Texture

	ProceduralTexture::ProceduralTexture()
	{
	}

	ProceduralTexture::~ProceduralTexture()
	{
	}

	void ProceduralTexture::Create(const void* data, uint32_t width, uint32_t height, Format format, uint32_t bpp)
	{
		Assert(!resource_ && data);
		width_ = width;
		height_ = height;
		depth_ = 1;
		format_ = format;

		DXGI_FORMAT dxgiFormat = GetDXGIFormat(format);

		// テクスチャ生成
		D3D11_TEXTURE2D_DESC objdesc;
		ZeroMemory(&objdesc, sizeof(objdesc));
		objdesc.Width = width;
		objdesc.Height = height;
		objdesc.MipLevels = 1;
		objdesc.ArraySize = 1;
		objdesc.SampleDesc.Count = 1;
		objdesc.SampleDesc.Quality = 0;
		objdesc.MiscFlags = 0;
		objdesc.Format = dxgiFormat;
		objdesc.Usage = D3D11_USAGE_DEFAULT;
		objdesc.CPUAccessFlags = 0;
		objdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		D3D11_SUBRESOURCE_DATA dataBin;
		dataBin.pSysMem = data;
		dataBin.SysMemPitch = bpp * width;
		dataBin.SysMemSlicePitch = 0;

		//	テクスチャ生成
		auto* device = GraphicsCore::GetDevice();
		ID3D11Texture2D* texture;
		THROW_IF_FAILED(device->CreateTexture2D(&objdesc, &dataBin, &texture));
		resource_ = texture;

		//	シェーダリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = dxgiFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		THROW_IF_FAILED(device->CreateShaderResourceView(texture, &srvDesc, &srv_));
	}


#pragma endregion

}