#pragma once

#include <windows.h>
#include <d3d11.h>

namespace se
{
	//
	//	アプリケーションウインドウ
	//
	class Window
	{
	private:
		static HINSTANCE				hInst_;				// インスタンスハンドル
		static HWND						hWnd_;					// ウィンドウハンドル

	public:
		static void Initialize(HINSTANCE hInst, int width, int height, const wchar_t* title);
		static void Show();
		static bool IsActive();
		static void SetWindowTitle(const wchar_t* name);
		static bool IsMinimized();
		static bool IsAlive();
		static void MessageLoop(MSG msg);

		static HWND GetHWND() { return hWnd_; }
	};

}