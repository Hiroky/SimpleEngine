#pragma once 

#include "se/Common.h"

namespace se
{
	enum BufferUsage
	{
		BUFFER_USAGE_IMMUTABLE,		// 不変バッファ
		BUFFER_USAGE_GPU_WRITE,		// GPUから書き込み可能
		BUFFER_USAGE_DYNAMIC,		// CPU, GPUから書き込み可能
	};

	enum IndexBufferStride
	{
		INDEX_BUFFER_STRIDE_U16,
		INDEX_BUFFER_STRIDE_U32,
	};

	struct VertexBufferDesc
	{
		const void* data;
		uint size;
		uint stride;
		bool canUnorderedAccess;
		BufferUsage usage;
	};


	//
	// GPUで使用されるバッファ
	//
	class GPUBuffer
	{
	protected:
		ID3D11Resource*		resource_;

	protected:
		void Destroy();

	public:
		GPUBuffer();
		virtual ~GPUBuffer();

		ID3D11Resource* GetResource() { return resource_; }

		template <class T>
		T* Get() const { return static_cast<T*>(resource_); }
	};



	//
	// 頂点バッファ
	//
	class VertexBuffer : public GPUBuffer
	{
	private:
		uint stride_;

	public:
		VertexBuffer();
		~VertexBuffer();

		void CreateBuffer(const VertexBufferDesc& desc);
		void DestroyBuffer();
		uint GetStride() const { return stride_; }
	};



	//
	// インデックスバッファ
	//
	class IndexBuffer : public GPUBuffer
	{
	private:
		IndexBufferStride stride_;
		uint bufferSize_;
		uint indexCount_;

	public:
		IndexBuffer();
		~IndexBuffer();

		void CreateBuffer(const void* data, uint size, IndexBufferStride stride);
		void DestroyBuffer();
	};

}