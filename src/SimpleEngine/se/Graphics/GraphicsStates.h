#pragma once 

#include "se/Common.h"
#include "se/Graphics/GraphicsCommon.h"
#include "se/Graphics/GraphicsContext.h"

namespace se
{

	class SamplerState
	{
		friend GraphicsContext;

	public:
		enum SamplerType
		{
			PointWrap,
			PointClamp,
			LinearWrap,
			LinearClamp,
			LinearComparisonClamp,
			AnisotropicClamp,
			AnisotropicWrap,
		};
		static const int SamplerTypeNum = AnisotropicWrap + 1;

	private:
		static SamplerState templates_[SamplerTypeNum];

	public:
		static void Initialize();
		static const SamplerState& Get(SamplerType type) { return templates_[type]; };
		//TODO:static const SamplerState& Get(Desc type);

	private:
		ID3D11SamplerState* state_;

	private:
		SamplerState();
		~SamplerState();
		SamplerState(const SamplerState&);	// コピー禁止
	};


	class DepthStencilState
	{
		friend GraphicsContext;

	public:
		enum DepthType
		{
			Disable,				// テストなし
			Enable,					// テストあり
			WriteDisable,			// デプス書き込みあり、比較なし
			WriteEnable,			// デプス書き込みあり、比較あり
			WriteEnableEqual,		// デプス書き込みあり、比較は等しい場合のみパス
			WriteEnableReverse,		// デプス書き込みあり、反対方向に比較あり
		};
		static const int DepthTypeNum = WriteEnableReverse + 1;

	private:
		static DepthStencilState templates_[DepthTypeNum];

	public:
		static void Initialize();
		static const DepthStencilState& Get(DepthType type) { return templates_[type]; };
		//TODO:static const DepthStencilState& Get(Desc type);

	private:
		ID3D11DepthStencilState* state_;

	private:
		DepthStencilState();
		~DepthStencilState();
		DepthStencilState(const DepthStencilState&);	// コピー禁止
	};


	class RasterizerState
	{
		friend GraphicsContext;

	public:
		enum RasterizerType
		{
			NoCull,
			BackFaceCull,
			FrontFaceCull,
			WireFrame,
		};
		static const int RasterizerTypeNum = WireFrame + 1;

	private:
		static RasterizerState templates_[RasterizerTypeNum];

	public:
		static void Initialize();
		static const RasterizerState& Get(RasterizerType type) { return templates_[type]; };
		//TODO:static const RasterizerState& Get(Desc type);

	private:
		ID3D11RasterizerState* state_;

	private:
		RasterizerState();
		~RasterizerState();
		RasterizerState(const RasterizerState&);	// コピー禁止
	};


	class BlendState
	{
	};
}