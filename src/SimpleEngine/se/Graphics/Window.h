#pragma once

#include <windows.h>

namespace se
{
	/**
	 *	マウスイベント情報
	 */
	struct MouseEventInfo
	{
		uint32_t x, y, wheel;
	};


	/**
	 *	アプリケーションウインドウ
	 */
	class Window
	{
	private:
		static HINSTANCE				hInst_;
		static HWND						hWnd_;

	public:
		static void Initialize(HINSTANCE hInst, int width, int height, const wchar_t* title);
		static void Show();
		static bool IsActive();
		static void SetWindowTitle(const wchar_t* name);
		static bool IsMinimized();
		static bool IsAlive();
		static void MessageLoop(MSG msg);

		static HWND GetHWND() { return hWnd_; }
		static const MouseEventInfo& GetMouseEventInfo();
		static void ResetMouseEventInfo();
	};

}