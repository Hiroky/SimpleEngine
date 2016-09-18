#pragma once 

#include "se/Common.h"
#include "se/HID/Mouse.h"

namespace se
{

	/**
	 * HIDコア機能
	 */
	class HIDCore
	{
	private:
		static Mouse mouse_;

	public:
		static void Initialize();
		static void Update();

		static const Mouse& GetMouse() { return mouse_; }
	};

}