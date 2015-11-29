﻿#pragma once 

#include "se/Common.h"

namespace se
{
	//
	// シェーダリフレクション
	//
	class ShaderReflection
	{
	private:
		ID3D11ShaderReflection* reflection_;

	public:
		ShaderReflection();
		~ShaderReflection();

		void Create(const void* data, size_t size);
		uint GetVertexLayoutAttribute();
		bool FindConstantBufferByName(const char* name, uint* out_bindIndex);
		bool FindUniformVariableByName(const char* name, uint* out_offset = NULL, uint* size = NULL);
		bool FindTextureBindByName(const char* name, uint* out_bindIndex);
	};

	//
	// 頂点レイアウト管理
	//
	class VertexLayoutManager
	{
	private:
		struct AttributeSet
		{
			uint vertexAttr;
			uint shaderAttr;
		};

	private:
		static ID3D11InputLayout* layouts_[64];
		static AttributeSet attributeSet_[64];
		static uint useCount_;

	public:
		void Initialize();
		void Finalize();

		ID3D11InputLayout* GetLayout(VertexShader* shader, uint vertexAttr);
	};

	//
	// 頂点シェーダ
	//
	class VertexShader
	{
		friend GraphicsContext;

	private:
		ID3D11VertexShader* shader_;
		ID3D11InputLayout* inputLayout_;
		ID3DBlob* blob_;
		const void* data_;
		size_t dataSize_;

	private:
		void CreateInputLayout(const void* data, size_t size);

	public:
		VertexShader();
		~VertexShader();

		ID3D11VertexShader* Get() const { return shader_; }
		void CreateFromByteCode(void* data, int size, ShaderReflection* reflection = nullptr);
		void CompileFromFile(const char* fileName, const char* entryPoint = "main", ShaderReflection* reflection = nullptr);
		void CompileFromString(const char* source, int length, const char* entryPoint = "main", ShaderReflection* reflection = nullptr);
	};

	//
	// ピクセルシェーダ
	//
	class PixelShader
	{
		friend GraphicsContext;

	private:
		ID3D11PixelShader* shader_;

	public:
		PixelShader();
		~PixelShader();

		ID3D11PixelShader* Get() const { return shader_; }
		void CreateFromByteCode(void* data, int size, ShaderReflection* reflection = nullptr);
		void CompileFromFile(const char* fileName, const char* entryPoint = "main", ShaderReflection* reflection = nullptr);
		void CompileFromString(const char* source, int length, const char* entryPoint = "main", ShaderReflection* reflection = nullptr);
	};


	//
	// シェーダセット
	//
	class ShaderSet
	{
	private:
		VertexShader vs_;
		PixelShader ps_;

	public:
		ShaderSet();
		~ShaderSet();

		VertexShader* GetVS() { return &vs_; }
		PixelShader* GetPS() { return &ps_; }

		void VSCompileFromString(const char* source, int length, const char* entryPoint = "main");
		void PSCompileFromString(const char* source, int length, const char* entryPoint = "main");
	};

}