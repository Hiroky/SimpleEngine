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
	// GPUで使用されるリソース
	//
	class GPUResource
	{
	protected:
		ID3D11Resource*				resource_;
		ID3D11ShaderResourceView*	srv_;
		ID3D11UnorderedAccessView*	uav_;


	protected:
		void Destroy();

	public:
		GPUResource();
		virtual ~GPUResource();

		ID3D11Resource* GetResource() const { return resource_; }
		ID3D11ShaderResourceView* GetSRV() const { return srv_; }
		ID3D11UnorderedAccessView* GetUAV() const { return uav_; }

		template <class T>
		T* Get() const { return static_cast<T*>(resource_); }
	};



	//
	// 頂点バッファ
	//
	class VertexBuffer : public GPUResource
	{
	private:
		uint stride_;

	public:
		VertexBuffer();
		virtual ~VertexBuffer();

		void CreateBuffer(const VertexBufferDesc& desc);
		void DestroyBuffer();
		uint GetStride() const { return stride_; }
	};



	//
	// インデックスバッファ
	//
	class IndexBuffer : public GPUResource
	{
	private:
		IndexBufferStride stride_;
		uint bufferSize_;
		uint indexCount_;

	public:
		IndexBuffer();
		virtual ~IndexBuffer();

		void CreateBuffer(const void* data, uint size, IndexBufferStride stride);
		void DestroyBuffer();
	};


	//
	// ピクセルバッファ
	//
	class PixelBuffer : public GPUResource
	{
	protected:
		uint width_;
		uint height_;
		uint depth_;
		uint format_;	// TODO:フォーマットEnum定義

	public:
		PixelBuffer();
		virtual ~PixelBuffer();

		uint GetWidth() const { return width_; }
		uint GetHeight() const { return height_; }
		uint GetDepth() const { return depth_; }
	};



	//
	// カラーバッファ
	//
	class ColorBuffer : public PixelBuffer
	{
	private:
		ID3D11RenderTargetView** rtvs_;
	public:
		ColorBuffer();
		virtual ~ColorBuffer();

		ID3D11RenderTargetView* GetRTV(uint index = 0) const { return rtvs_[index]; }
	};


	//
	// デプスステンシルバッファ
	//
	class DepthStencilBuffer : public PixelBuffer
	{
	private:
		ID3D11DepthStencilView* dsv_;

	public:
		DepthStencilBuffer();
		virtual ~DepthStencilBuffer();

		ID3D11DepthStencilView* GetDSV() const { return dsv_; }
	};


	//
	// テクスチャリソース
	//
	class Texture : PixelBuffer
	{
	public:
		Texture();
		virtual ~Texture();

		void LoadFromFile(const char* fileName);
		void LoadFromMemory(const void* data, uint size);
	};
}