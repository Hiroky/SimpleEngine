#include "se/Graphics/GraphicsStates.h"
#include "se/Graphics/GraphicsCore.h"


namespace se
{
#pragma region SamplerState

	SamplerState SamplerState::templates_[SamplerTypeNum];

	void SamplerState::Initialize()
	{
		auto* device = GraphicsCore::GetDevice();

		{
			D3D11_SAMPLER_DESC sampDesc;

			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.MipLODBias = 0.0f;
			sampDesc.MaxAnisotropy = 1;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			HRESULT hr = device->CreateSamplerState(&sampDesc, &templates_[PointWrap].state_);
			THROW_IF_FAILED(hr);
		}
		{
			D3D11_SAMPLER_DESC sampDesc;

			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.MipLODBias = 0.0f;
			sampDesc.MaxAnisotropy = 1;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			HRESULT hr = device->CreateSamplerState(&sampDesc, &templates_[PointClamp].state_);
			THROW_IF_FAILED(hr);
		}
		{
			D3D11_SAMPLER_DESC sampDesc;

			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.MipLODBias = 0.0f;
			sampDesc.MaxAnisotropy = 1;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			HRESULT hr = device->CreateSamplerState(&sampDesc, &templates_[LinearWrap].state_);
			THROW_IF_FAILED(hr);
		}
		{
			D3D11_SAMPLER_DESC sampDesc;

			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.MipLODBias = 0.0f;
			sampDesc.MaxAnisotropy = 1;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			HRESULT hr = device->CreateSamplerState(&sampDesc, &templates_[LinearClamp].state_);
			THROW_IF_FAILED(hr);
		}
	}

	SamplerState::SamplerState()
		: state_(nullptr)
	{
	}

	SamplerState::~SamplerState()
	{
		COMPTR_RELEASE(state_)
	}

#pragma endregion

#pragma region

	DepthStencilState DepthStencilState::templates_[DepthTypeNum];

	void DepthStencilState::Initialize()
	{
		auto* device = GraphicsCore::GetDevice();

		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace = dsDesc.FrontFace;

		{
			dsDesc.DepthEnable = false;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&dsDesc, &templates_[Disable].state_);
			THROW_IF_FAILED(hr);
		}
		{
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&dsDesc, &templates_[Enable].state_);
			THROW_IF_FAILED(hr);
		}
		{
			dsDesc.DepthEnable = false;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&dsDesc, &templates_[WriteDisable].state_);
			THROW_IF_FAILED(hr);
		}
		{
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&dsDesc, &templates_[WriteEnable].state_);
			THROW_IF_FAILED(hr);
		}
		{
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&dsDesc, &templates_[WriteEnableEqual].state_);
			THROW_IF_FAILED(hr);
		}
		{
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			HRESULT hr = device->CreateDepthStencilState(&dsDesc, &templates_[WriteEnableReverse].state_);
			THROW_IF_FAILED(hr);
		}
	}

	DepthStencilState::DepthStencilState()
		: state_(nullptr)
	{
	}

	DepthStencilState::~DepthStencilState()
	{
		COMPTR_RELEASE(state_)
	}

#pragma endregion

#pragma region RasterizerState

	RasterizerState RasterizerState::templates_[RasterizerTypeNum];

	void RasterizerState::Initialize()
	{
		auto* device = GraphicsCore::GetDevice();

		D3D11_RASTERIZER_DESC rastDesc;
		rastDesc.AntialiasedLineEnable = false;
		rastDesc.DepthBias = 0;
		rastDesc.DepthBiasClamp = 0.0f;
		rastDesc.DepthClipEnable = true;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.MultisampleEnable = false;
		rastDesc.FrontCounterClockwise = true;	// 右手座標系
		rastDesc.ScissorEnable = true;

		{
			rastDesc.CullMode = D3D11_CULL_NONE;
			rastDesc.FillMode = D3D11_FILL_SOLID;
			HRESULT hr = device->CreateRasterizerState(&rastDesc, &templates_[NoCull].state_);
			THROW_IF_FAILED(hr);
		}
		{
			rastDesc.CullMode = D3D11_CULL_BACK;
			rastDesc.FillMode = D3D11_FILL_SOLID;
			HRESULT hr = device->CreateRasterizerState(&rastDesc, &templates_[BackFaceCull].state_);
			THROW_IF_FAILED(hr);
		}
		{
			rastDesc.CullMode = D3D11_CULL_FRONT;
			rastDesc.FillMode = D3D11_FILL_SOLID;
			HRESULT hr = device->CreateRasterizerState(&rastDesc, &templates_[FrontFaceCull].state_);
			THROW_IF_FAILED(hr);
		}
		{
			rastDesc.CullMode = D3D11_CULL_NONE;
			rastDesc.FillMode = D3D11_FILL_WIREFRAME;
			HRESULT hr = device->CreateRasterizerState(&rastDesc, &templates_[WireFrame].state_);
			THROW_IF_FAILED(hr);
		}
	}

	RasterizerState::RasterizerState()
		: state_(nullptr)
	{
	}

	RasterizerState::~RasterizerState()
	{
		COMPTR_RELEASE(state_)
	}

#pragma endregion

}