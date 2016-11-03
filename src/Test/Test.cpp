#include "se/engine.h"

namespace {
	struct Vertex
	{
		se::float3 pos;
		se::float2 uv;
	};

	se::TUniformParameter<se::ViewParameterData> viewUniforms;
	se::TUniformParameter<se::ObjectParameterData> objectUniforms;
	se::ColorBuffer temporalBuffer[3];
	uint32_t currentBufferIndex = 0;
	se::Texture texture;

	// ハルトンシーケンス
	float HaltonSequence(uint32_t index, uint32_t base)
	{
		float result = 0.0f;
		float invBase = 1.0f / base;
		float fraction = invBase;
		while (index > 0) {
			result += (index % base) * fraction;
			index /= base;
			fraction *= invBase;
		}
		return result;
	}
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

	// TemporalAA
	for (uint32_t i = 0; i < 3; i++) {
		temporalBuffer[i].Create2D(se::FORMAT_R8G8B8A8_UNORM, se::GraphicsCore::GetDisplayWidth(), se::GraphicsCore::GetDisplayHeight());
	}
	currentBufferIndex = 0;
	auto* temporalAAShader = se::ShaderManager::Get().Find("TemporalAA");
	auto* quadShader = se::ShaderManager::Get().Find("ScreenQuad");
	Assert(temporalAAShader && quadShader);

	// 頂点データ
	Vertex vertex[] = {
		{ se::float3(-1.0f, 1.0f, 0), se::float2(0.0f, 0.0f) },
		{ se::float3(-1.0f, -3.0f, 0), se::float2(0.0f, 2.0f) },
		{ se::float3(3.0f, 1.0f, 0), se::float2(2.0f, 0.0f) },
	};
	uint16_t index[] = { 0, 1, 2 };
	se::VertexBuffer vertexBuffer;
	se::IndexBuffer indexBuffer;
	vertexBuffer.Create(vertex, sizeof(vertex), se::VERTEX_ATTR_FLAG_POSITION | se::VERTEX_ATTR_FLAG_TEXCOORD0);
	indexBuffer.Create(index, sizeof(index), se::INDEX_BUFFER_STRIDE_U16);
	const auto* layout = se::VertexLayoutManager::Get().FindLayout(*quadShader->GetVS(), vertexBuffer.GetAttributes());

	// テクスチャ
	texture.LoadFromFile("texture/test.dds");

	// メッシュ
	se::StaticMesh mesh;
	mesh.Create("mesh/sponza/sponza.obj");
	//mesh.Create("mesh/cube/cube.obj");

	se::Camera camera;
	camera.SetLookAt(se::float3(3, 2, 0), se::float3(5, 2, 0), se::float3(0, 1, 0));
	camera.SetPerspective(16.0f / 9.0f, se::DegreeToRadian(45.0f), 0.1f, 1000.0f);
	camera.Build();
	se::CameraController cameraController(camera);
	viewUniforms.Contents().worldToView = se::float4x4::Transpose(camera.GetView());
	viewUniforms.Contents().viewToClip = se::float4x4::Transpose(camera.GetProjection());
	viewUniforms.Contents().worldToClip = se::float4x4::Transpose(camera.GetViewProjection());
	objectUniforms.Contents().localToWorld = se::float4x4::ScaleMatrix(se::float3(0.01f));

	auto* meshShader = se::ShaderManager::Get().Find("OneTexture");
	Assert(meshShader);
	const auto* meshLayout = se::VertexLayoutManager::Get().FindLayout(*meshShader->GetVS(), mesh.GetVertexBuffer().GetAttributes());

	uint32_t jitterIndex = 0;

	// メインループ
	MSG msg = { 0 };
	while (se::Window::IsAlive()) {
		if(!se::Window::IsMinimized()) {

			/**
			 * メイン 
			 */
			se::HIDCore::Update();
			cameraController.Update();

			/**
			 * 描画
			 */
			auto& context = se::GraphicsCore::GetImmediateContext();
			se::GPUProfiler::Get().BeginFrameProfiling(context);
			{
				seGpuPerfScope(context, 0, "main");
				auto& colorBuffer = se::GraphicsCore::GetDisplayColorBuffer();
				auto& depthBuffer = se::GraphicsCore::GetDisplayDepthStencilBuffer();
				auto& currentBuffer = temporalBuffer[2];

				context.SetRenderTarget(&currentBuffer, 1, &depthBuffer);
				context.ClearRenderTarget(currentBuffer, se::float4(0.3f, 0.4f, 0.9f, 1.0f));
				context.ClearDepthStencil(depthBuffer);
				context.SetViewportAndScissorRect(se::Rect(0, 0, colorBuffer.GetWidth(), colorBuffer.GetHeight()));

				// temporal camera jitter
				auto projection = camera.GetProjection();
				float x = HaltonSequence(jitterIndex, 2) * 2.0f - 1.0f;
				float y = HaltonSequence(jitterIndex, 3) * 2.0f - 1.0f;
				jitterIndex = (jitterIndex + 1) & 0x7;
				projection.m[2][0] = -x / se::GraphicsCore::GetDisplayWidth();
				projection.m[2][1] = -y / se::GraphicsCore::GetDisplayHeight();
				auto viewProjection = camera.GetView() * projection;
				viewUniforms.Contents().worldToClip = se::float4x4::Transpose(viewProjection);
				viewUniforms.Updated();
				viewUniforms.Update(context);

				// 3D render
				{
					seGpuPerfScope(context, 0, "3DRender");
					objectUniforms.Update(context);
					context.SetVertexShader(*meshShader->GetVS());
					context.SetPixelShader(*meshShader->GetPS());
					context.SetInputLayout(*meshLayout);
					context.SetVertexBuffer(0, &mesh.GetVertexBuffer());
					context.SetIndexBuffer(&mesh.GetIndexBuffer());
					context.SetVSConstantBuffer(0, viewUniforms.GetResource());
					context.SetVSConstantBuffer(1, objectUniforms.GetResource());
					context.SetBlendState(se::BlendState::Get(se::BlendState::Opaque));
					context.SetDepthStencilState(se::DepthStencilState::Get(se::DepthStencilState::WriteEnable));
					context.SetRasterizerState(se::RasterizerState::Get(se::RasterizerState::BackFaceCull));
					context.SetPrimitiveType(se::PRIMITIVE_TYPE_TRIANGLE_LIST);
					context.SetPSSamplerState(0, se::SamplerState::Get(se::SamplerState::AnisotropicWrap));
					for (uint32_t i = 0; i < mesh.GetShapeNum(); i++) {
						const auto& shape = mesh.GetShape(i);
						const auto& material = mesh.GetMaterial(shape.materialIndex);
						if (material.albedo) {
							context.SetPSResource(0, material.albedo);
						}
						context.DrawIndexed(shape.indexStart, shape.indexCount);
					}
				}

				// TemporalAA
				{
					seGpuPerfScope(context, 0, "TemporalAA");
					context.SetRenderTarget(&temporalBuffer[currentBufferIndex], 1, nullptr);
					context.SetVertexShader(*temporalAAShader->GetVS());
					context.SetPixelShader(*temporalAAShader->GetPS());
					context.SetInputLayout(*layout);
					context.SetVertexBuffer(0, &vertexBuffer);
					context.SetIndexBuffer(&indexBuffer);
					context.SetPSResource(0, &currentBuffer);
					context.SetPSResource(1, &temporalBuffer[currentBufferIndex ^ 1]);
					context.SetPSSamplerState(0, se::SamplerState::Get(se::SamplerState::LinearClamp));
					context.SetBlendState(se::BlendState::Get(se::BlendState::Opaque));
					context.SetDepthStencilState(se::DepthStencilState::Get(se::DepthStencilState::Disable));
					context.SetRasterizerState(se::RasterizerState::Get(se::RasterizerState::NoCull));
					context.SetPrimitiveType(se::PRIMITIVE_TYPE_TRIANGLE_LIST);
					context.DrawIndexed(0, 3);
					context.SetPSResource(0, &se::ColorBuffer());
					context.SetPSResource(1, &se::ColorBuffer());

					// to displaybuffer
					context.SetRenderTarget(&colorBuffer, 1, nullptr);
					context.SetVertexShader(*quadShader->GetVS());
					context.SetPixelShader(*quadShader->GetPS());
					context.SetPSResource(0, &temporalBuffer[currentBufferIndex]);
					context.DrawIndexed(0, 3);

					currentBufferIndex ^= 1;
				}
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
