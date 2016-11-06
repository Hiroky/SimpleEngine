#pragma once 

#include "se/Common.h"
#include "se/Graphics/GraphicsCommon.h"
#include "se/Graphics/GraphicsContext.h"

namespace se
{
	enum BufferUsage
	{
		BUFFER_USAGE_DEFAULT,
		BUFFER_USAGE_IMMUTABLE,		// 不変バッファ
		BUFFER_USAGE_GPU_WRITE,		// GPUから書き込み可能
		BUFFER_USAGE_DYNAMIC,		// CPU, GPUから書き込み可能
	};

	enum IndexBufferStride
	{
		INDEX_BUFFER_STRIDE_U16,
		INDEX_BUFFER_STRIDE_U32,
	};


	/**
	 * コンスタントバッファ
	 */
	class ConstantBuffer
	{
		friend class GraphicsContext;

	private:
		ID3D11Buffer* buffer_;
		uint32_t size_;

	public:
		ConstantBuffer();
		virtual ~ConstantBuffer();

		void Create(uint32_t size, BufferUsage usage);
		void Update(GraphicsContext& context, const void* data, uint32_t size);
	};


	/**
	 * GPUで使用されるリソース
	 */
	class GPUResource
	{
	protected:
		ID3D11Resource*				resource_;
		ID3D11ShaderResourceView*	srv_;
		ID3D11UnorderedAccessView*	uav_;

	public:
		GPUResource();
		virtual ~GPUResource();
		virtual void Destroy();

		ID3D11Resource* GetResource() const { return resource_; }
		ID3D11ShaderResourceView* GetSRV() const { return srv_; }
		ID3D11UnorderedAccessView* GetUAV() const { return uav_; }

		template <class T>
		T* Get() const { return static_cast<T*>(resource_); }

		bool IsCreated() const { return resource_ != nullptr; }
	};

	
	/**
	 * 頂点バッファ
	 */
	class VertexBuffer : public GPUResource
	{
	private:
		uint32_t stride_;
		uint32_t attributes_;
		uint32_t size_;

	public:
		VertexBuffer();
		virtual ~VertexBuffer();

		void Create(const void* data, uint32_t size, VertexAttributeFlags attributes, BufferUsage usage = BUFFER_USAGE_IMMUTABLE, bool unorderedAccess = false);
		void Create(uint32_t vertexNum, VertexAttributeFlags attributes, BufferUsage usage = BUFFER_USAGE_IMMUTABLE, bool unorderedAccess = false);
		virtual void Destroy() override;

		uint32_t GetStride() const { return stride_; }
		uint32_t GetAttributes() const { return attributes_; }
		uint32_t GetSize() const { return size_; }
		uint32_t GetVertexNum() const { return size_ / stride_; }
	};

	
	/**
	 * インデックスバッファ
	 */
	class IndexBuffer : public GPUResource
	{
	private:
		IndexBufferStride stride_;
		uint32_t bufferSize_;
		uint32_t indexCount_;

	public:
		IndexBuffer();
		virtual ~IndexBuffer();

		void Create(const void* data, uint32_t size, IndexBufferStride stride, BufferUsage usage = BUFFER_USAGE_IMMUTABLE);
		void Create(uint32_t num, BufferUsage usage = BUFFER_USAGE_IMMUTABLE);
		virtual void Destroy() override;

		IndexBufferStride GetStride() const { return stride_; }
		uint32_t GetIndexNum() const { return indexCount_; }
	};


	/**
	 * ピクセルバッファ
	 */
	class PixelBuffer : public GPUResource
	{
	protected:
		uint32_t width_;
		uint32_t height_;
		uint32_t depth_;
		Format format_;

	public:
		PixelBuffer();
		virtual ~PixelBuffer();

		uint32_t GetWidth() const { return width_; }
		uint32_t GetHeight() const { return height_; }
		uint32_t GetDepth() const { return depth_; }
	};
	

	/**
	 * カラーバッファ
	 */
	class ColorBuffer : public PixelBuffer
	{
		friend class GraphicsCore;

	private:
		ID3D11RenderTargetView* rtv_;

	private:
		void InitializeDisplayBuffer(ID3D11RenderTargetView* renderTarget);

	public:
		ColorBuffer();
		virtual ~ColorBuffer();

		void Create2D(Format format, uint32_t width, uint32_t height, uint32_t arraySize = 1u, uint32_t mips = 1u, bool unorderedAccess = false);
		// TODO:各種初期化対応
		//void Initialize3D();
		//void InitializeCube();

		virtual void Destroy() override;

		ID3D11RenderTargetView* GetRTV() const { return rtv_; }
	};


	/**
	 * デプスステンシルバッファ
	 */
	class DepthStencilBuffer : public PixelBuffer
	{
	private:
		ID3D11DepthStencilView* dsv_;

	public:
		DepthStencilBuffer();
		virtual ~DepthStencilBuffer();

		void Create(uint32_t width, uint32_t height);
		virtual void Destroy() override;

		ID3D11DepthStencilView* GetDSV() const { return dsv_; }
	};


	/**
	 * テクスチャリソース
	 */
	class Texture : public PixelBuffer
	{
	public:
		Texture();
		virtual ~Texture();

		void LoadFromFile(const char* fileName);
		void LoadFromMemory(const void* data, uint32_t size);
	};


	/**
	 * CPUからアクセス可能なリソース
	 */
	class ProceduralTexture : public PixelBuffer
	{
	public:
		ProceduralTexture();
		virtual ~ProceduralTexture();

		void Create(const void* data, uint32_t width, uint32_t height, Format format, uint32_t bpp);
	};


	/**
	* クエリ
	*/
	class Query
	{
		friend class GraphicsContext;

	public:
		enum Type {
			TIMESTAMP,
			TIMESTAMP_DISJOINT,

			UNKNOWN
		};

	private:
		ID3D11Query* query_;
		Type type_;

	public:
		Query();
		~Query();

		void Create(Type type);
		void Destroy();
		bool IsCreated() const { return query_ != nullptr; }
	};


	/**
	 * ユニフォームパラメータ
	 */
	template <class T>
	class TUniformParameter
	{
	private:
		ConstantBuffer resource_;
		T contents_;
		bool isCreated_;
		bool isUpdated_;

	public:
		TUniformParameter()
			: isCreated_(false)
			, isUpdated_(true)
		{
		}

		void Destroy()
		{
			resource_.Destroy();
			isCreated_ = false;
		}

		void Set(const T& buffer)
		{
			contents_ = buffer;
			isUpdated_ = true;
		}

		void Update(GraphicsContext& context, bool forceUpdate = false)
		{
			if (!isCreated_) {
				resource_.Create(sizeof(T), BUFFER_USAGE_DEFAULT);
				isCreated_ = true;
			}
			if (isUpdated_ || forceUpdate) {
				resource_.Update(context, &contents_, sizeof(T));
				isUpdated_ = false;
			}
		}

		T& Contents() { return contents_; }
		const T& Contents() const { return contents_; }
		void Updated() { isUpdated_ = true; }
		const ConstantBuffer& GetResource() const { return resource_; }
		bool IsCreated() const { return isCreated_; }
	};
}