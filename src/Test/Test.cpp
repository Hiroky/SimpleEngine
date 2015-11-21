#include "se/Graphics/Window.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	se::Window::Initialize(hInstance, 1280, 720, L"SimpleEngine");
	se::GraphicsCore::Initialize();
	se::Window::Show();

	// ÉÅÉCÉìÉãÅ[Év
	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			se::GraphicsCore::Present(1, 0);
		}
	}
	se::GraphicsCore::Finalize();

    return (int) msg.wParam;
}
