#include "se/Graphics/Window.h"

const char* vs_string = "\
struct Vertex\
{\
	float4 a_position : POSITION;\
	float2 a_texcoord0: TEXCOORD0;\
};\
\
struct PSInput\
{\
	float4 v_position : SV_POSITION;\
	float2 v_texcoord0: TEXCOORD0;\
};\
\
PSInput main(Vertex v)\
{\
	PSInput o;\
	o.v_position = v.a_position;\
	o.v_texcoord0 = v.a_texcoord0;\
	return o;\
}\
";

const char* ps_string = "\
Texture2D texture0 : register(t0);\
SamplerState sampler0 : register(s0);\
struct PSInput\
{\
	float4 v_position : SV_POSITION;\
	float2 v_texcoord0: TEXCOORD0;\
};\
\
float4 main(PSInput i) : SV_Target\
{\
	return texture0.Sample(sampler0, i.v_texcoord0);\
}\
";

struct Vertex
{
	se::float3 pos;
	se::float2 uv;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	se::Window::Initialize(hInstance, 1280, 720, L"SimpleEngine");
	se::GraphicsCore::Initialize();
	se::Window::Show();

	se::ShaderSet shader;
	shader.VSCompileFromString(vs_string, static_cast<int>(strlen(vs_string)));
	shader.PSCompileFromString(ps_string, static_cast<int>(strlen(ps_string)));

	Vertex vertex[] = {
		{ se::float3(0, 0.8f, 0), se::float2(0.5f, 0.0f) },
		{ se::float3(-0.5f, -0.8f, 0), se::float2(0.0f, 1.0f) },
		{ se::float3(0.5f, -0.8f, 0), se::float2(1.0f, 1.0f) },
	};
	uint index[] = { 0, 1, 2 };
	se::VertexBuffer vertexBuffer;
	se::VertexBufferDesc vbdesc = { 
		vertex,
		sizeof(vertex),
		se::VERTEX_ATTR_POSITION | se::VERTEX_ATTR_TEXCOORD0,
		false,
		se::BUFFER_USAGE_IMMUTABLE 
	};
	vertexBuffer.CreateBuffer(vbdesc);
	vertexBuffer.SetupVertexLayout(*shader.GetVS());
	se::IndexBuffer indexBuffer;
	indexBuffer.CreateBuffer(index, sizeof(index), se::INDEX_BUFFER_STRIDE_U32);

	se::Texture texture;
	texture.LoadFromFile("test.dds");

	// ƒƒCƒ“ƒ‹[ƒv
	MSG msg = { 0 };
	while (se::Window::IsAlive()) {
		if(!se::Window::IsMinimized()) {
			auto* context = se::GraphicsCore::GetImmediateContext();
			auto& colorBuffer = se::GraphicsCore::GetDisplayColorBuffer();
			auto& depthBuffer = se::GraphicsCore::GetDisplayDepthStencilBuffer();

			context->SetRenderTarget(&colorBuffer, 1, &depthBuffer);
			context->ClearRenderTarget(colorBuffer, se::float4(0.3f, 0.4f, 0.9f, 1.0f));
			context->ClearDepthStencil(depthBuffer);

			context->SetVertexShader(*shader.GetVS());
			context->SetPixelShader(*shader.GetPS());
			context->SetVertexBuffer(0, &vertexBuffer);
			context->SetIndexBuffer(&indexBuffer);
			context->SetPSResource(0, &texture);
			context->SetPSSamplerState(0, se::SamplerState::Get(se::SamplerState::LinearClamp));
			context->DrawIndexed(0, 3);

			se::GraphicsCore::Present(1, 0);
		}
		se::Window::MessageLoop(msg);
	}

	se::GraphicsCore::Finalize();
    return (int) msg.wParam;
}
