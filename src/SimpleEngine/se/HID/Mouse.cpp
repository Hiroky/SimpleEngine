#include "se/HID/Mouse.h"
#include "se/Graphics/Window.h"

namespace se {

	void Mouse::Update()
	{
		//マウス移動値取得
		auto& info = Window::GetMouseEventInfo();
		MouseState backup = prevState_;
		prevState_ = state_;
		state_.x = info.x;
		state_.y = info.y;
		state_.lx = info.x - backup.x;
		state_.ly = info.y - backup.y;
		state_.lm = info.wheel;

		//マウスボタン入力判定
		static const uint32_t sMouseButton[] = {
			VK_LBUTTON,
			VK_RBUTTON,
			VK_MBUTTON,
		};
		static const uint32_t sMouseFlg[] = {
			static_cast<uint32_t>(MouseButton::LEFT),
			static_cast<uint32_t>(MouseButton::RIGHT),
			static_cast<uint32_t>(MouseButton::MIDDLE)
		};

		prevButtonFlg_ = buttonFlg_;
		buttonFlg_ = 0;
		for (uint32_t i = 0; i < 3; i++) {
			int16_t flg = GetAsyncKeyState(sMouseButton[i]);
			if (flg & 0x8000) {
				buttonFlg_ |= sMouseFlg[i];
			}
		}

		
	}

}