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


	void ShaderReflection::Create(void* data, size_t size)
	{
		HRESULT hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection_);
		THROW_IF_FAILED(hr);
	}


	//
	//	頂点アトリビュートを取得する
	//
	uint ShaderReflection::GetVertexLayoutAttribute()
	{
#if 0
		static const char* input_semantics[] = {
			"POSITION",
			"NORMAL",
			"TEXCOORD",
			"WEIGHT",
			"BONEINDEX",
		};
		static const uint input_attribute[] = {
			ksShaderManager::KS_SHADER_ATTR_POS,
			ksShaderManager::KS_SHADER_ATTR_NORMAL,
			ksShaderManager::KS_SHADER_ATTR_TEXCOORD0,
			ksShaderManager::KS_SHADER_ATTR_SKIN,
			ksShaderManager::KS_SHADER_ATTR_SKIN,
		};

		uint attr = 0;
		D3D11_SHADER_DESC shader_desc;
		reflection_->GetDesc(&shader_desc);
		for (uint i = 0; i < shader_desc.InputParameters; i++) {
			D3D11_SIGNATURE_PARAMETER_DESC param_desc;
			reflection_->GetInputParameterDesc(i, &param_desc);
			for (int j = 0; j < KS_ARRAY_SIZE(input_semantics); j++) {
				if (strcmp(input_semantics[j], param_desc.SemanticName) == 0) {
					attr |= input_attribute[j];
					break;
				}
			}
		}
		return attr;
#else
		return 0;
#endif
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


	// === VertexShader ====================================================================================

	VertexShader::VertexShader()
		: shader_(nullptr)
		, inputLayout_(nullptr)
	{
	}

	VertexShader::~VertexShader()
	{
		COMPTR_RELEASE(shader_);
		COMPTR_RELEASE(inputLayout_);
	}

	void VertexShader::CreateInputLayout(const void * data, size_t size)
	{
		// 仮
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HRESULT hr = GraphicsCore::GetDevice()->CreateInputLayout(
			layout,
			1,
			data,
			size,
			&inputLayout_);
	}

	void VertexShader::CreateFromByteCode(void* data, int size, ShaderReflection* reflection)
	{
		HRESULT hr = GraphicsCore::GetDevice()->CreateVertexShader(data, size, nullptr, &shader_);
		THROW_IF_FAILED(hr);
		CreateInputLayout(data, size);

		if (reflection) {
			reflection->Create(data, size);
		}
	}

	void VertexShader::CompileFromFile(const char* fileName, const char* entryPoint, ShaderReflection* reflection)
	{
		ID3DBlob* blob = nullptr;
		CompileShaderFromFile(fileName, entryPoint, "vs_5_0", &blob);
		HRESULT hr = GraphicsCore::GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader_);
		THROW_IF_FAILED(hr);
		CreateInputLayout(blob->GetBufferPointer(), blob->GetBufferSize());

		if (reflection) {
			reflection->Create(blob->GetBufferPointer(), blob->GetBufferSize());
		}
		COMPTR_RELEASE(blob);
	}

	void VertexShader::CompileFromString(const char* source, int length, const char* entryPoint, ShaderReflection* reflection)
	{
		ID3DBlob* blob = nullptr;
		CompileShaderFromString(source, length, entryPoint, "vs_5_0", &blob);
		HRESULT hr = GraphicsCore::GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader_);
		THROW_IF_FAILED(hr);
		CreateInputLayout(blob->GetBufferPointer(), blob->GetBufferSize());

		if (reflection) {
			reflection->Create(blob->GetBufferPointer(), blob->GetBufferSize());
		}
		COMPTR_RELEASE(blob);
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

	void PixelShader::CreateFromByteCode(void* data, int size, ShaderReflection* reflection)
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