#include "se/Graphics/GraphicsContext.h"
#include "Shader.h"


namespace se
{
	namespace
	{
		void CompileShaderFromFile(const char* szFileName, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlobOut)
		{
			HRESULT hr = S_OK;
			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
			dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

			ID3DBlob* pErrorBlob;
			size_t size = 0;
			wchar_t buffer[128] = { 0 };
			size_t chr_len = strlen(szFileName) + 1;
			mbstowcs_s(&size, buffer, chr_len, szFileName, _TRUNCATE);

			// シェーダをファイルからコンパイル
			hr = D3DCompileFromFile(
				buffer,
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				szEntryPoint,
				szShaderModel,
				dwShaderFlags,
				0,
				ppBlobOut,
				&pErrorBlob
				);

			// エラーメッセージ出力
			if (FAILED(hr)) {
				if (pErrorBlob != NULL) {
					OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
					throw;
				}
			}

			COMPTR_RELEASE(pErrorBlob);
		}

		void CompileShaderFromString(const char* str, int length, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlobOut)
		{
			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
			dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
			ID3DBlob* pErrorBlob;

			// シェーダをファイルからコンパイル
			HRESULT hr = D3DCompile(
				str,
				length,
				nullptr,
				nullptr,
				nullptr,
				szEntryPoint,
				szShaderModel,
				dwShaderFlags,
				0,
				ppBlobOut,
				&pErrorBlob
				);

			// エラーメッセージ出力
			if (FAILED(hr)) {
				if (pErrorBlob != NULL) {
					OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
					throw;
				}
			}

			COMPTR_RELEASE(pErrorBlob);
		}
	}


	// === ShaderReflection ====================================================================================

	ShaderReflection::ShaderReflection()
		: reflection_(nullptr)
	{
	}


	ShaderReflection::~ShaderReflection()
	{
		COMPTR_RELEASE(reflection_);
	}


	void ShaderReflection::Create(const void* data, size_t size)
	{
		HRESULT hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection_);
		THROW_IF_FAILED(hr);
	}


	//
	//	頂点アトリビュートを取得する
	//
	uint ShaderReflection::GetVertexLayoutAttribute()
	{
		static const std::array<char*, 7> inputSemantics = {
			"POSITION",
			"NORMAL",
			"TEXCOORD",
			"TANGENT",
			"BITANGENT",
			"BLENDWEIGHT",
			"BLENDINDECES",
		};
		static const std::array<uint, 7> inputAttribute = {
			VERTEX_ATTR_POSITION,
			VERTEX_ATTR_NORMAL,
			VERTEX_ATTR_TEXCOORD0,
			VERTEX_ATTR_TANGENT,
			VERTEX_ATTR_BITANGENT,
			VERTEX_ATTR_BLENDWEIGHT,
			VERTEX_ATTR_BLENDINDECES,
		};

		uint attr = 0;
		D3D11_SHADER_DESC shader_desc;
		reflection_->GetDesc(&shader_desc);
		for (uint i = 0; i < shader_desc.InputParameters; i++) {
			D3D11_SIGNATURE_PARAMETER_DESC param_desc;
			reflection_->GetInputParameterDesc(i, &param_desc);
			for (uint j = 0; j < inputSemantics.size(); j++) {
				if (strcmp(inputSemantics[j], param_desc.SemanticName) == 0) {
					attr |= inputAttribute[j];
					break;
				}
			}
		}
		return attr;
	}


	//	
	//	名前からコンスタントバッファを検索する
	//
	bool ShaderReflection::FindConstantBufferByName(const char* name, uint* out_bindIndex)
	{
		D3D11_SHADER_INPUT_BIND_DESC desc;
		HRESULT hr = reflection_->GetResourceBindingDescByName(name, &desc);
		if (SUCCEEDED(hr)) {
			if (out_bindIndex) {
				*out_bindIndex = desc.BindPoint;
			}
			return true;
		}

		return false;
	}

	//	
	//
	bool ShaderReflection::FindTextureBindByName(const char* name, uint* out_bindIndex)
	{
		D3D11_SHADER_INPUT_BIND_DESC desc;
		HRESULT hr = reflection_->GetResourceBindingDescByName(name, &desc);
		if (SUCCEEDED(hr) && !(desc.uFlags & D3D_SIF_UNUSED)) {
			if (out_bindIndex) {
				*out_bindIndex = desc.BindPoint;
			}
			return true;
		}

		return false;
	}

	//
	//	名前からシェーダ内で使用されているコンスタントバッファ内の変数を検索する
	//
	bool ShaderReflection::FindUniformVariableByName(const char* name, uint* out_offset, uint* out_size)
	{
		ID3D11ShaderReflectionVariable* var = reflection_->GetVariableByName(name);
		D3D11_SHADER_VARIABLE_DESC desc;
		HRESULT hr = var->GetDesc(&desc);
		if (SUCCEEDED(hr) && (desc.uFlags & D3D_SVF_USED)) {
#if 0
			ID3D11ShaderReflectionConstantBuffer* buffer = var->GetBuffer();
			D3D11_SHADER_BUFFER_DESC buffer_desc;
			buffer->GetDesc(&buffer_desc);
#endif
			//	バッファ開始オフセット
			if (out_offset) {
				*out_offset = desc.StartOffset;
			}
			//	変数サイズ
			if (out_size) {
				*out_size = desc.Size;
			}
			return true;
		}
		return false;
	}

	// === VertexLayoutManager ====================================================================================
	ID3D11InputLayout* VertexLayoutManager::layouts_[64];
	VertexLayoutManager::AttributeSet VertexLayoutManager::attributeSet_[64];
	uint VertexLayoutManager::useCount_;


	void VertexLayoutManager::Initialize()
	{
		memset(layouts_, 0, sizeof(layouts_));
		memset(attributeSet_, 0, sizeof(attributeSet_));
		useCount_ = 0;
	}

	void VertexLayoutManager::Finalize()
	{
		for (uint i = 0; i < useCount_; i++) {
			COMPTR_RELEASE(layouts_[i]);
		}
		useCount_ = 0;
	}

	ID3D11InputLayout * VertexLayoutManager::GetLayout(const VertexShader& shader, uint vertexAttr)
	{
		// 既存データから検索
		uint i;
		for (i = 0; i < useCount_; i++) {
			if (vertexAttr == attributeSet_[i].vertexAttr
				|| shader.GetVertexAttribute() == attributeSet_[i].shaderAttr
				) {
				break;
			}
		}

		// 見つからなかったら生成
		if (i == useCount_) {
			D3D11_INPUT_ELEMENT_DESC desc[16];
			uint count = 0;
			uint offset = 0;
			if (vertexAttr & VERTEX_ATTR_POSITION) {
				D3D11_INPUT_ELEMENT_DESC t = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 12;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_NORMAL) {
				D3D11_INPUT_ELEMENT_DESC t = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 12;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_COLOR) {
				D3D11_INPUT_ELEMENT_DESC t = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 16;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_TEXCOORD0) {
				D3D11_INPUT_ELEMENT_DESC t = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 8;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_TEXCOORD1) {
				D3D11_INPUT_ELEMENT_DESC t = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 8;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_TEXCOORD2) {
				D3D11_INPUT_ELEMENT_DESC t = { "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 8;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_TEXCOORD3) {
				D3D11_INPUT_ELEMENT_DESC t = { "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 8;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_TANGENT) {
				D3D11_INPUT_ELEMENT_DESC t = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 12;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_BITANGENT) {
				D3D11_INPUT_ELEMENT_DESC t = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 12;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_BLENDWEIGHT) {
				D3D11_INPUT_ELEMENT_DESC t = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 12;
				count++;
			}
			if (vertexAttr & VERTEX_ATTR_BLENDINDECES) {
				D3D11_INPUT_ELEMENT_DESC t = { "BLENDINDECES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				desc[count] = t;
				offset += 12;
				count++;
			}

			HRESULT hr = GraphicsCore::GetDevice()->CreateInputLayout(desc, count, shader.GetByteCode(), shader.GetBiteCodeSize(), &layouts_[useCount_]);
			THROW_IF_FAILED(hr);
			attributeSet_[useCount_].shaderAttr = shader.GetVertexAttribute();
			attributeSet_[useCount_].vertexAttr = vertexAttr;
			useCount_++;
		}

		return layouts_[i];
	}

	// === VertexShader ====================================================================================

	VertexShader::VertexShader()
		: shader_(nullptr)
		, blob_(nullptr)
		, data_(nullptr)
		, dataSize_(0)
	{
	}

	VertexShader::~VertexShader()
	{
		COMPTR_RELEASE(shader_);
		COMPTR_RELEASE(blob_);
	}

	void VertexShader::CreateFromByteCode(const void* data, int size, ShaderReflection* reflection)
	{
		HRESULT hr = GraphicsCore::GetDevice()->CreateVertexShader(data, size, nullptr, &shader_);
		THROW_IF_FAILED(hr);
		data_ = data;
		dataSize_ = size;

		ShaderReflection ref;
		ref.Create(data_, dataSize_);
		vertexAttribute_ = ref.GetVertexLayoutAttribute();

		if (reflection) {
			reflection->Create(data, size);
		}
	}

	void VertexShader::CompileFromFile(const char* fileName, const char* entryPoint, ShaderReflection* reflection)
	{
		CompileShaderFromFile(fileName, entryPoint, "vs_5_0", &blob_);
		HRESULT hr = GraphicsCore::GetDevice()->CreateVertexShader(blob_->GetBufferPointer(), blob_->GetBufferSize(), nullptr, &shader_);
		THROW_IF_FAILED(hr);
		data_ = blob_->GetBufferPointer();
		dataSize_ = blob_->GetBufferSize();

		ShaderReflection ref;
		ref.Create(data_, dataSize_);
		vertexAttribute_ = ref.GetVertexLayoutAttribute();

		if (reflection) {
			reflection->Create(data_, dataSize_);
		}
	}

	void VertexShader::CompileFromString(const char* source, int length, const char* entryPoint, ShaderReflection* reflection)
	{
		CompileShaderFromString(source, length, entryPoint, "vs_5_0", &blob_);
		HRESULT hr = GraphicsCore::GetDevice()->CreateVertexShader(blob_->GetBufferPointer(), blob_->GetBufferSize(), nullptr, &shader_);
		THROW_IF_FAILED(hr);
		data_ = blob_->GetBufferPointer();
		dataSize_ = blob_->GetBufferSize();

		ShaderReflection ref;
		ref.Create(data_, dataSize_);
		vertexAttribute_ = ref.GetVertexLayoutAttribute();

		if (reflection) {
			reflection->Create(data_, dataSize_);
		}
	}


	// === PixelShader ====================================================================================

	PixelShader::PixelShader()
		: shader_(nullptr)
	{
	}

	PixelShader::~PixelShader()
	{
		COMPTR_RELEASE(shader_);
	}

	void PixelShader::CreateFromByteCode(const void* data, int size, ShaderReflection* reflection)
	{
		HRESULT hr = GraphicsCore::GetDevice()->CreatePixelShader(data, size, nullptr, &shader_);
		THROW_IF_FAILED(hr);

		if (reflection) {
			reflection->Create(data, size);
		}
	}

	void PixelShader::CompileFromFile(const char* fileName, const char* entryPoint, ShaderReflection* reflection)
	{
		ID3DBlob* blob = nullptr;
		CompileShaderFromFile(fileName, entryPoint, "ps_5_0", &blob);
		HRESULT hr = GraphicsCore::GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader_);
		THROW_IF_FAILED(hr);

		if (reflection) {
			reflection->Create(blob->GetBufferPointer(), blob->GetBufferSize());
		}
		COMPTR_RELEASE(blob);
	}

	void PixelShader::CompileFromString(const char* source, int length, const char* entryPoint, ShaderReflection* reflection)
	{
		ID3DBlob* blob = nullptr;
		CompileShaderFromString(source, length, entryPoint, "ps_5_0", &blob);
		HRESULT hr = GraphicsCore::GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader_);
		THROW_IF_FAILED(hr);

		if (reflection) {
			reflection->Create(blob->GetBufferPointer(), blob->GetBufferSize());
		}
		COMPTR_RELEASE(blob);
	}


	// === ShaderSet ====================================================================================

	ShaderSet::ShaderSet()
	{
	}

	ShaderSet::~ShaderSet()
	{
	}

	void ShaderSet::VSCompileFromString(const char * source, int length, const char * entryPoint)
	{
		vs_.CompileFromString(source, length, entryPoint);
	}

	void ShaderSet::PSCompileFromString(const char * source, int length, const char * entryPoint)
	{
		ps_.CompileFromString(source, length, entryPoint);
	}

}