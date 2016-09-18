#include "se/Debug/ImplImgui.h"
#include "se/HID/HIDCore.h"

namespace se {

	namespace {
		INT64 g_Time = 0;
		INT64 g_TicksPerSecond = 0;
		const ShaderSet* g_shader = nullptr;
		const VertexInputLayout* g_vertexLayout = nullptr;
		ProceduralTexture fontTexture;
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;

		// コンスタントバッファ
		struct ImguiConstants
		{
			float mvp[4][4];
		};
		TUniformParameter<ImguiConstants> g_constants;
	}


	/**
	 * 描画ハンドラ
	 */
	void ImGuiRenderDrawLists(ImDrawData* draw_data)
	{
		auto& context = GraphicsCore::GetImmediateContext();

		// 頂点バッファ
		if (!vertexBuffer.GetResource() || vertexBuffer.GetVertexNum() < (uint32_t)draw_data->TotalVtxCount) {
			vertexBuffer.Destroy();
			uint32_t vertexCount = draw_data->TotalVtxCount + 5000;
			vertexBuffer.Create(vertexCount, VERTEX_ATTR_FLAG_POSITION | VERTEX_ATTR_FLAG_TEXCOORD0 | VERTEX_ATTR_FLAG_BYTE_COLOR, BUFFER_USAGE_DYNAMIC);
			g_vertexLayout = se::VertexLayoutManager::Get().FindLayout(*g_shader->GetVS(), vertexBuffer.GetAttributes());
		}
		// インデックスバッファ
		if (!indexBuffer.GetResource() || indexBuffer.GetIndexNum() < (uint32_t)draw_data->TotalIdxCount) {
			indexBuffer.Destroy();
			uint32_t indexCount = draw_data->TotalIdxCount + 10000;
			indexBuffer.Create(indexCount, BUFFER_USAGE_DYNAMIC);
		}

		// バッファ構築
		void* vertexPtr = context.Map(vertexBuffer);
		void* indexPtr = context.Map(indexBuffer);
		ImDrawVert* vtx_dst = (ImDrawVert*)vertexPtr;
		ImDrawIdx* idx_dst = (ImDrawIdx*)indexPtr;
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtx_dst += cmd_list->VtxBuffer.Size;
			idx_dst += cmd_list->IdxBuffer.Size;
		}
		context.Unmap(vertexBuffer);
		context.Unmap(indexBuffer);


		// コンスタントバッファ
		{
			float L = 0.0f;
			float R = ImGui::GetIO().DisplaySize.x;
			float B = ImGui::GetIO().DisplaySize.y;
			float T = 0.0f;
			float mvp[4][4] =
			{
				{ 2.0f / (R - L),		0.0f,					0.0f,       0.0f },
				{ 0.0f,					2.0f / (T - B),			0.0f,       0.0f },
				{ 0.0f,					0.0f,					0.5f,       0.0f },
				{ (R + L) / (L - R),	(T + B) / (B - T),		0.5f,       1.0f },
			};
			memcpy(g_constants.Contents().mvp, mvp, sizeof(mvp));
			g_constants.Update(context, true);
		}

		// Setup
		context.SetViewport(Rect(0, 0, (int32_t)ImGui::GetIO().DisplaySize.x, (int32_t)ImGui::GetIO().DisplaySize.y));
		context.SetVertexShader(*g_shader->GetVS());
		context.SetPixelShader(*g_shader->GetPS());
		context.SetVertexBuffer(0, &vertexBuffer);
		context.SetInputLayout(*g_vertexLayout);
		context.SetIndexBuffer(&indexBuffer);
		context.SetPrimitiveType(PRIMITIVE_TYPE_TRIANGLE_LIST);
		context.SetVSConstantBuffer(0, g_constants.GetResource());
		context.SetPSSamplerState(0, SamplerState::Get(SamplerState::LinearWrap));
		context.SetDepthStencilState(DepthStencilState::Get(DepthStencilState::Disable));
		context.SetRasterizerState(RasterizerState::Get(RasterizerState::NoCull));
		context.SetBlendState(BlendState::Get(BlendState::Translucent));

		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback) {
					pcmd->UserCallback(cmd_list, pcmd);
				} else {
					const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
					context.SetPSResource(0, (GPUResource*)pcmd->TextureId);
					context.SetScissorRect(Rect((LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w));
					context.DrawIndexed(idx_offset, pcmd->ElemCount, vtx_offset);
				}
				idx_offset += pcmd->ElemCount;
			}
			vtx_offset += cmd_list->VtxBuffer.Size;
		}
	}

	/**
	 * ウインドウプロシージャ
	 */
	IMGUI_API LRESULT ImGuiWndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		ImGuiIO& io = ImGui::GetIO();
		switch (msg)
		{
		case WM_KEYDOWN:
			if (wParam < 256)
				io.KeysDown[wParam] = 1;
			return true;
		case WM_KEYUP:
			if (wParam < 256)
				io.KeysDown[wParam] = 0;
			return true;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if (wParam > 0 && wParam < 0x10000)
				io.AddInputCharacter((unsigned short)wParam);
			return true;
		}
		return 0;
	}


	/**
	 * 初期化
	 */
	bool ImGuiInitialize()
	{
		if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
			return false;
		if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
			return false;

		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';

		io.RenderDrawListsFn = ImGuiRenderDrawLists;
		io.ImeWindowHandle = Window::GetHWND();

		// shader setup
		g_shader = ShaderManager::Get().Find("RenderImgui");
		Assert(g_shader);

		// Build texture atlas
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		fontTexture.Create(pixels, width, height, FORMAT_R8G8B8A8_UNORM, 4);
		io.Fonts->TexID = (void*)&fontTexture;

		return true;
	}


	/**
	 * 終了処理
	 */
	void ImGuiShutdown()
	{
		ImGui::Shutdown();
	}


	/**
	 * 新規フレーム開始
	 */
	void ImGuiNewFrame()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Setup display size (every frame to accommodate for window resizing)
		io.DisplaySize = ImVec2((float)GraphicsCore::GetDisplayWidth(), (float)GraphicsCore::GetDisplayHeight());

		// Setup time step
		INT64 current_time;
		QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
		io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
		g_Time = current_time;

		// Read keyboard modifiers inputs
		io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
		io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
		io.KeySuper = false;

		auto& mouse = HIDCore::GetMouse();
		auto& mouseInfo = mouse.GetMouseInfo();
		io.MouseWheel += mouseInfo.lm;
		io.MousePos.x = static_cast<float>(mouseInfo.x);
		io.MousePos.y = static_cast<float>(mouseInfo.y);
		io.MouseDown[0] = mouse.IsDown(static_cast<uint32_t>(MouseButton::LEFT));
		io.MouseDown[1] = mouse.IsDown(static_cast<uint32_t>(MouseButton::RIGHT));
		io.MouseDown[2] = mouse.IsDown(static_cast<uint32_t>(MouseButton::MIDDLE));

		// Start the frame
		ImGui::NewFrame();
	}


	/**
	 * 描画
	 */
	void ImGuiRender()
	{
		ImGui::Render();
	}

}