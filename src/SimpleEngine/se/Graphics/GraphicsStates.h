#pragma once 

#include "se/Common.h"

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
}