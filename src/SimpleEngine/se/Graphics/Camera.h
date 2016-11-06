#pragma once 

#include "se/Common.h"
#include "se/Math/Math.h"
#include "se/HID/HIDCore.h"

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
		float3 at_;
		float3 up_;
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

		const float3& GetPosition() const { return position_; }
		const float3& GetAt() const { return at_; }
		const float3& GetUp() const { return up_; }
	};


	/**
	 * カメラ操作
	 */
	class CameraController
	{
	private:
		Camera& camera_;
		float3 at_;
		float3 rotation_;
		float length_;

		const float MIN_ROTATION_X = DegreeToRadian(-89);
		const float MAX_ROTATION_X = DegreeToRadian(89);
		const float ROTATE_UNIT = 0.2f;
		const float ZOOM_UNIT = 0.01f;
		const float TRANCE_UNIT = 0.01f;

	public:
		CameraController(Camera& camera)
			: camera_(camera)
			, rotation_(0, 0, 0)
		{
			float3 dir = camera_.GetPosition() - camera_.GetAt();
			length_ = dir.Length();
			at_ = camera_.GetAt();

			// 初期計算
			ReCalcCamera();
		}


		void Update()
		{
			bool is_update = false;
			const auto& mouse = HIDCore::GetMouse();

			// 左クリック
			if (mouse.IsDown((uint32_t)MouseButton::LEFT)) {
				is_update = true;

				// マウス操作取得
				float2 move;
				move.x = (float)mouse.GetMouseInfo().lx;
				move.y = (float)mouse.GetMouseInfo().ly;

				// 回転
				rotation_.x += DegreeToRadian(-move.y * ROTATE_UNIT);
				rotation_.x = Clamp<float>(rotation_.x, MIN_ROTATION_X, MAX_ROTATION_X);
				rotation_.y += DegreeToRadian(-move.x * ROTATE_UNIT);
			} else if (mouse.IsDown((uint32_t)MouseButton::RIGHT)) {
				// 右クリック
				is_update = true;

				float2 move;
				move.x = (float)mouse.GetMouseInfo().lx;
				move.y = (float)mouse.GetMouseInfo().ly;
				length_ += move.x * ZOOM_UNIT;
				length_ += -move.y * ZOOM_UNIT;
				length_ = Max(0.01f, length_);
			} else if (mouse.IsDown((uint32_t)MouseButton::MIDDLE)) {
				// ホイール
				is_update = true;

				float2 move;
				move.x = (float)mouse.GetMouseInfo().lx;
				move.y = (float)mouse.GetMouseInfo().ly;
				Vector3 result;
				Vector3 tmp = camera_.GetAt() - camera_.GetPosition();
				tmp.Normalize();
				Vector3 up = camera_.GetUp();
				result = Vector3::Cross(tmp, up);
				result.Normalize();
				up = Vector3::Cross(tmp, result);
				up.Normalize();

				at_ -= result * move.x * TRANCE_UNIT;
				at_ -= up * move.y * TRANCE_UNIT;
			}

			// 更新があった場合位置を再計算
			if (is_update) {
				ReCalcCamera();
			}
		}

		void ReCalcCamera()
		{
			Quaternion q(XMQuaternionRotationRollPitchYaw(rotation_.x, rotation_.y, 0));

			float3 new_dir = float3::Transform(float3(0, 0, 1), q);
			new_dir *= length_;

			float3 pos;
			pos.x = new_dir.x + at_.x;
			pos.y = new_dir.y + at_.y;
			pos.z = new_dir.z + at_.z;
			camera_.SetLookAt(pos, at_, float3(0, 1, 0));
		}

		void SetAspect(float aspect)
		{
			//camera_.SetAspect(aspect);
			//camera_.Update();
		}
	};
}