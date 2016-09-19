#include "se/engine.h"

namespace {
	struct Vertex
	{
		se::float3 pos;
		se::float2 uv;
	};

	se::Texture texture;
}

void RenderProfileTree(const se::GPUProfiler::ProfilerTreeMember* member, uint32_t layer)
{
	static char margin[32] = { 0 };
	for (uint32_t i = 0; i < layer; i++) {
		margin[i] = '\t';
	}
	margin[layer] = 0;
	ImGui::Text("%s%s: %.3fms", margin, member->name_, (float)member->time_);

	// 子も表示
	for (auto* c : member->childMembers_) {
		RenderProfileTree(c, layer + 1);
	}
}

void ProcImgui()
{
	se::ImGuiNewFrame();

	{
		// テストウインドウ
		static bool open = true;
		ImGuiWindowFlags window_flags = 0;
		if (!ImGui::Begin("ImGui Demo", &open, window_flags)) {
			ImGui::End();
			return;
		}

		ImGui::Text("Dear ImGui says hello.");

		float width = ImGui::GetContentRegionAvailWidth();
		ImGui::Image(&texture, se::IMVec2(width, width * ((float)texture.GetHeight() / texture.GetWidth())));

		ImGui::End();
	}

	// GPUプロファイラ
	{
		static bool gpuProfilerView = true;
		const auto* profileTree = se::GPUProfiler::Get().GetProfilerTreeRootNode();

		if (!ImGui::Begin("GPU Profile", &gpuProfilerView, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
			ImGui::End();
			return;
		}

		ImGui::Text("GPU Profiling");
		//ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		ImGui::Separator();
		if (profileTree) {
			RenderProfileTree(profileTree, 0);
		}
		ImGui::End();
	}

	se::ImGuiRender();
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	se::Window::Initialize(hInstance, 1600, 900, L"SimpleEngine");	// 900p
	se::GraphicsCore::Initialize();
	se::ShaderManager::Get().Initialize("./shaders");
	se::HIDCore::Initialize();
	se::Window::Show();

	// ImGui
	se::ImGuiInitialize();
	se::ImGuiAddFontFromFileTTF("./Roboto-Regular.ttf", 14.0f);

	// 描画用シェーダ
	auto* quadShader = se::ShaderManager::Get().Find("ScreenQuad");
	Assert(quadShader);

	// 頂点データ
	Vertex vertex[] = {
		{ se::float3(-1.0f, 1.0f, 0), se::float2(0.0f, 0.0f) },
		{ se::float3(-1.0f, -3.0f, 0), se::float2(0.0f, 2.0f) },
		{ se::float3(3.0f, 1.0f, 0), se::float2(2.0f, 0.0f) },
	};
	uint32_t index[] = { 0, 1, 2 };
	se::VertexBuffer vertexBuffer;
	se::IndexBuffer indexBuffer;
	vertexBuffer.Create(vertex, sizeof(vertex), se::VERTEX_ATTR_FLAG_POSITION | se::VERTEX_ATTR_FLAG_TEXCOORD0);
	indexBuffer.Create(index, sizeof(index), se::INDEX_BUFFER_STRIDE_U32);
	const auto* layout = se::VertexLayoutManager::Get().FindLayout(*quadShader->GetVS(), vertexBuffer.GetAttributes());

	// テクスチャ
	texture.LoadFromFile("texture/test.dds");

	// メインループ
	MSG msg = { 0 };
	while (se::Window::IsAlive()) {
		if(!se::Window::IsMinimized()) {

			/* メイン */

			se::HIDCore::Update();


			/* 描画 */

			auto& context = se::GraphicsCore::GetImmediateContext();
			auto& colorBuffer = se::GraphicsCore::GetDisplayColorBuffer();
			auto& depthBuffer = se::GraphicsCore::GetDisplayDepthStencilBuffer();

			se::GPUProfiler::Get().BeginFrameProfiling(context);

			{
				seGpuPerfScope(context, 0, "main");
				context.SetRenderTarget(&colorBuffer, 1, &depthBuffer);
				context.ClearRenderTarget(colorBuffer, se::float4(0.3f, 0.4f, 0.9f, 1.0f));
				context.ClearDepthStencil(depthBuffer);
				context.SetViewportAndScissorRect(se::Rect(0, 0, colorBuffer.GetWidth(), colorBuffer.GetHeight()));

				context.SetVertexShader(*quadShader->GetVS());
				context.SetPixelShader(*quadShader->GetPS());
				context.SetInputLayout(*layout);
				context.SetVertexBuffer(0, &vertexBuffer);
				context.SetIndexBuffer(&indexBuffer);
				context.SetPSResource(0, &texture);
				context.SetPSSamplerState(0, se::SamplerState::Get(se::SamplerState::LinearWrap));
				context.SetBlendState(se::BlendState::Get(se::BlendState::Opaque));
				context.SetDepthStencilState(se::DepthStencilState::Get(se::DepthStencilState::Disable));
				context.SetRasterizerState(se::RasterizerState::Get(se::RasterizerState::NoCull));
				context.SetPrimitiveType(se::PRIMITIVE_TYPE_TRIANGLE_LIST);
				context.DrawIndexed(0, 3);
			}
			{
				seGpuPerfScope(context, 0, "imgui");
				ProcImgui();
			}

			se::GPUProfiler::Get().EndFrameProfiling(context);
			se::GraphicsCore::Present(1, 0);
		}
		se::Window::MessageLoop(msg);
	}

	se::GraphicsCore::Finalize();
    return (int) msg.wParam;
}
