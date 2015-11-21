#include "Window.h"

namespace se 
{
	namespace 
	{
		//ウインドウプロシージャ
		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
		{
			switch (uMsg) {
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc;
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

			default:
			{
				/* DO_NOTHING */
			}
			break;
			}

			return DefWindowProc(hWnd, uMsg, wp, lp);
		}
	}


	HINSTANCE Window::hInst_;				// インスタンスハンドル
	HWND Window::hWnd_;					// ウィンドウハンドル


	void Window::Initialize(HINSTANCE hInst, int width, int height, const wchar_t* title)
	{
		// 拡張ウィンドウクラスの登録
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = title;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wc)) {
			throw;
		}

		// インスタンスハンドルを設定
		hInst_ = hInst;

		// ウィンドウサイズの設定
		RECT rc = { 0, 0, width, height };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		// ウィンドウの生成
		hWnd_ = CreateWindow(
			title,
			title,
			WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			NULL,
			NULL,
			hInst,
			NULL);

		if (!hWnd_) {
			throw;
		}
	}


	void Window::Show()
	{
		ShowWindow(hWnd_, SW_SHOWDEFAULT);
	}


	bool Window::IsActive()
	{
		return hWnd_ == GetActiveWindow();
	}


	void Window::SetWindowTitle(const wchar_t* name)
	{
		SetWindowText(hWnd_, name);
	}

}