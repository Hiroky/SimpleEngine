#include "se/Graphics/Atmosphere.h"
#include "se/Debug/ImplImgui.h"

namespace se
{
	namespace {
		ComputeShader* computeTransmittanceCS = nullptr;
	}

	Atmosphere::Atmosphere()
	{
	}

	void Atmosphere::Initialize()
	{
		computeTransmittanceCS = ShaderManager::Get().FindCompute("ComputeTransmittanceLUT");
		Assert(computeTransmittanceCS);
	}

	void Atmosphere::Render(GraphicsContext& context)
	{
		if(!transmittanceLUT_.IsCreated()) {
			transmittanceLUT_.Create2D(FORMAT_R16G16B16A16_FLOAT, 256, 64, 1, 1, true);

			context.SetComputeShader(*computeTransmittanceCS);
			context.SetCSUnorderedAccessView(0, &transmittanceLUT_);
			context.Dispatch(TG(transmittanceLUT_.GetWidth(), 16), TG(transmittanceLUT_.GetHeight(), 16), 1);
			context.SetCSUnorderedAccessView(0, &se::GPUResource());
		}
	}

	void Atmosphere::DebugGUI()
	{
		static bool open = true;
		ImGuiWindowFlags window_flags = 0;
		if (!ImGui::Begin("Atmosphere", &open, window_flags)) {
			ImGui::End();
		} else {
			float width = ImGui::GetContentRegionAvailWidth();
			ImGui::Image(&transmittanceLUT_, se::IMVec2(width, width * ((float)transmittanceLUT_.GetHeight() / transmittanceLUT_.GetWidth())));
			ImGui::End();
		}
	}
}