#include "Camera.h"


namespace se
{
	Camera::Camera()
		: near_(0)
		, far_(0)
		, aspect_(0)
		, fov_(0)
		, isUpdated_(true)
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::Build()
	{
		if (!isUpdated_) return;
		viewProjection_ = view_ * projection_;
	}

	void Camera::SetLookAt(const float3& eye, const float3& lookat, const float3& up)
	{
		view_ = XMMatrixLookAtRH(eye.ToSIMD(), lookat.ToSIMD(), up.ToSIMD());
		world_ = float4x4::Invert(view_);
		position_ = eye;
	}
	void Camera::SetPerspective(float aspectRatio, float fieldOfView, float nearClip, float farClip)
	{
		projection_ = XMMatrixPerspectiveFovRH(fieldOfView, aspectRatio, nearClip, farClip);
		near_ = nearClip;
		far_ = farClip;
		aspect_ = aspectRatio;
		fov_ = fieldOfView;
	}
}