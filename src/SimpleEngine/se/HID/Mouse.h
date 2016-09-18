#pragma once 

#include "se/Common.h"

namespace se
{

	/**
	 * マウスボタンフラグ
	 */
	enum class MouseButton {
		LEFT	= 1 << 0,
		RIGHT	= 1 << 1,
		MIDDLE	= 1 << 2
	};


	/**
	 * マウスステート
	 */
	struct MouseState
	{
		uint32_t x, y;		// 現在位置
		uint32_t lx, ly;	// 移動量
		float wheel;

		void Crear() {
			x = y = lx = ly = 0;
			wheel = 0;
		}
	};


	/**
	 * マウス
	 */
	class Mouse
	{
	private:
		uint32_t buttonFlg_;
		uint32_t prevButtonFlg_;
		MouseState state_;
		MouseState prevState_;

	public:
		Mouse()
			: buttonFlg_(0)
			, prevButtonFlg_(0)
		{
			state_.Crear();
			prevState_.Crear();
		}

		void Update();

		bool IsDown(uint32_t key) const { return (buttonFlg_ & key) != 0; }
		bool IsTrigger(uint32_t key) const { return (buttonFlg_ & (~prevButtonFlg_) & key) != 0; }
		const MouseState& GetMouseInfo() const { return state_; }
	};

}