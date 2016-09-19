#pragma once 

#include "se/Common.h"

namespace se
{
	class Camera
	{
	private:
		float4x4 projection_;
		float4x4 view_;
		float4x4 viewProjection_;
		float4x4 world_;
		float3 position_;
		float near_;
		float far_;
		float fov_;
		float aspect_;
		bool isUpdated_;

	public:
		Camera();
		~Camera();

		void Build();
		void SetLookAt(const float3& eye, const float3& lookat, const float3& up);
		void SetPerspective(float aspectRatio, float fieldOfView, float nearClip, float farClip);

		const float4x4& GetProjection() const { return projection_; }
		const float4x4& GetView() const { return view_; }
		const float4x4& GetViewProjection() const { return viewProjection_; }
		float GetNear() const { return near_; }
		float GetFar() const { return far_; }
	};
}