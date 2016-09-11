#include "se/Graphics/Window.h"


struct Vertex
{
	se::float3 pos;
	se::float2 uv;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	se::Window::Initialize(hInstance, 1280, 720, L"SimpleEngine");
	se::GraphicsCore::Initialize();
	se::ShaderManager::Get().Initialize("shaders");
	se::Window::Show();

	// 描画用シェーダ
	auto* quadShader = se::ShaderManager::Get().Find("ScreenQuad");
	Assert(quadShader);

	// 頂点データ
	Vertex vertex[] = {
		{ se::float3(-1.0f,  1.0f, 0), se::float2(0.0f, 0.0f) },
		{ se::float3(-1.0f, -1.0f, 0), se::float2(0.0f, 1.0f) },
		{ se::float3( 1.0f, -1.0f, 0), se::float2(1.0f, 1.0f) },
		{ se::float3( 1.0f,  1.0f, 0), se::float2(1.0f, 0.0f) },
	};
	uint32_t index[] = { 0, 1, 2, 0, 2, 3 };
	se::VertexBuffer vertexBuffer;
	se::IndexBuffer indexBuffer;
	vertexBuffer.Create(vertex, sizeof(vertex), se::VERTEX_ATTR_FLAG_POSITION | se::VERTEX_ATTR_FLAG_TEXCOORD0);
	indexBuffer.Create(index, sizeof(index), se::INDEX_BUFFER_STRIDE_U32);
	const auto* layout = se::VertexLayoutManager::Get().FindLayout(*quadShader->GetVS(), vertexBuffer.GetAttributes());

	// テクスチャ
	se::Texture texture;
	texture.LoadFromFile("texture/test.dds");

	// メインループ
	MSG msg = { 0 };
	while (se::Window::IsAlive()) {
		if(!se::Window::IsMinimized()) {
			auto* context = se::GraphicsCore::GetImmediateContext();
			auto& colorBuffer = se::GraphicsCore::GetDisplayColorBuffer();
			auto& depthBuffer = se::GraphicsCore::GetDisplayDepthStencilBuffer();

			context->SetRenderTarget(&colorBuffer, 1, &depthBuffer);
			context->ClearRenderTarget(colorBuffer, se::float4(0.3f, 0.4f, 0.9f, 1.0f));
			context->ClearDepthStencil(depthBuffer);

			context->SetVertexShader(*quadShader->GetVS());
			context->SetPixelShader(*quadShader->GetPS());
			context->SetInputLayout(*layout);
			context->SetVertexBuffer(0, &vertexBuffer);
			context->SetIndexBuffer(&indexBuffer);
			context->SetPSResource(0, &texture);
			context->SetPSSamplerState(0, se::SamplerState::Get(se::SamplerState::LinearClamp));
			context->SetDepthStencilState(se::DepthStencilState::Get(se::DepthStencilState::Disable));
			context->SetRasterizerState(se::RasterizerState::Get(se::RasterizerState::NoCull));
			context->DrawIndexed(0, 6);

			se::GraphicsCore::Present(1, 0);
		}
		se::Window::MessageLoop(msg);
	}

	se::GraphicsCore::Finalize();
    return (int) msg.wParam;
}
