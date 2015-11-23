#include "se/Graphics/Window.h"

const char* vs_string = "\
struct Vertex\
{\
	float4 a_position : POSITION;\
};\
\
struct PSInput\
{\
	float4 v_position : SV_POSITION;\
};\
\
PSInput main(Vertex v)\
{\
	PSInput o;\
	o.v_position = v.a_position;\
	return o;\
}\
";

const char* ps_string = "\
struct PSInput\
{\
	float4 v_position : SV_POSITION;\
};\
\
float4 main(PSInput i) : SV_Target\
{\
	return float4(1, 1, 1, 1);\
}\
";

class Vector3
{
public:
	union
	{
		struct
		{
			float x, y, z;
		};
		float v[3];
	};

	Vector3() {}
	Vector3(float x, float y, float z)
		: x(x), y(y), z(z)
	{
	}
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
	shader.VSCompileFromString(vs_string, strlen(vs_string));
	shader.PSCompileFromString(ps_string, strlen(ps_string));

	Vector3 vertex[] = { Vector3(0, 1, 0), Vector3(-0.5f, -1.0f, 0), Vector3(0.5f, -1.0f, 0) };
	uint index[] = { 0, 1, 2 };
	se::VertexBuffer vertexBuffer;
	se::VertexBufferDesc vbdesc = { vertex, sizeof(vertex), 12, false, se::BUFFER_USAGE_IMMUTABLE };
	vertexBuffer.CreateBuffer(vbdesc);
	se::IndexBuffer indexBuffer;
	indexBuffer.CreateBuffer(index, sizeof(index), se::INDEX_BUFFER_STRIDE_U32);

	// ƒƒCƒ“ƒ‹[ƒv
	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			auto* context = se::GraphicsCore::GetImmediateContext();

			se::GraphicsCore::ClearRenderTarget();

			context->SetVertexShader(*shader.GetVS());
			context->SetPixelShader(*shader.GetPS());
			context->SetVertexBuffer(0, &vertexBuffer);
			context->SetIndexBuffer(&indexBuffer);
			context->DrawIndexed(0, 3);

			se::GraphicsCore::Present(1, 0);
		}
	}

	se::GraphicsCore::Finalize();

    return (int) msg.wParam;
}
