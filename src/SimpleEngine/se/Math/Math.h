#pragma once 

#include <DirectXMath.h>
using namespace DirectX;

namespace se
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Quaternion;
	struct Matrix4x4;

	/**
	 * Vector2
	 */
	struct Vector2 : public XMFLOAT2
	{
		Vector2() {}
		Vector2(float xy)
			: XMFLOAT2(xy, xy)
		{
		}
		Vector2(float x, float y)
			: XMFLOAT2(x, y)
		{
		}
		Vector2(const XMFLOAT2& xy)
			: XMFLOAT2(xy)
		{
		}
		Vector2(FXMVECTOR v)
		{
			XMStoreFloat2(this, v);
		}

		Vector2& operator*=(const Vector2& other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}
		Vector2 operator*(const Vector2& other) const
		{
			Vector2 result;
			result.x = x * other.x;
			result.y = y * other.y;
			return result;
		}
		Vector2& operator*=(float s)
		{
			x *= s;
			y *= s;
			return *this;
		}
		Vector2 operator*(float s) const
		{
			Vector2 result;
			result.x = x * s;
			result.y = y * s;
			return result;
		}

		FXMVECTOR ToSIMD() const { return XMLoadFloat2(this); }

		float* ToFloatArray() { return reinterpret_cast<float*>(this); }
		const float* ToFloatArray() const { return reinterpret_cast<const float*>(this); }
	};
	typedef Vector2 float2;

	/**
	 * Vector3
	 */
	struct Vector3 : public XMFLOAT3
	{
		Vector3(){}
		Vector3(float xyz)
			: XMFLOAT3(xyz, xyz, xyz)
		{
		}
		Vector3(float x, float y, float z)
			: XMFLOAT3(x, y, z)
		{
		}
		Vector3(const XMFLOAT3& xyz)
			: XMFLOAT3(xyz)
		{
		}
		Vector3(FXMVECTOR v)
		{
			XMStoreFloat3(this, v);
		}

		Vector3& operator*=(const Vector3& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}
		Vector3 operator*(const Vector3& other) const
		{
			Vector3 result;
			result.x = x * other.x;
			result.y = y * other.y;
			result.z = z * other.z;
			return result;
		}
		Vector3& operator*=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		Vector3 operator*(float s) const
		{
			Vector3 result;
			result.x = x * s;
			result.y = y * s;
			result.z = z * s;
			return result;
		}
		Vector3 operator-(const Vector3& other) const
		{
			Vector3 result;
			result.x = x - other.x;
			result.y = y - other.y;
			result.z = z - other.z;
			return result;
		}

		const Vector3& operator-=(const Vector3& other)
		{
			x = x - other.x;
			y = y - other.y;
			z = z - other.z;
			return *this;
		}

		float Length() const
		{
			float length;
			XMVectorGetXPtr(&length, XMVector3Length(ToSIMD()));
			return length;
		}

		void Normalize()
		{
			auto res = XMVector3Normalize(ToSIMD());
			XMStoreFloat3(this, res);
		}

		void Transform(const Matrix4x4& m);
		void Transform(const Quaternion& q);

		FXMVECTOR ToSIMD() const { return XMLoadFloat3(this); }

		float* ToFloatArray() { return reinterpret_cast<float*>(this); }
		const float* ToFloatArray() const { return reinterpret_cast<const float*>(this); }

	public:
		static Vector3 Transform(const Vector3& v, const Matrix4x4& m);
		static Vector3 Transform(const Vector3& v, const Quaternion& q);
		static Vector3 Cross(const Vector3& a, const Vector3& b);
	};
	typedef Vector3 float3;


	/**
	 * Vector4
	 */
	struct Vector4 : public XMFLOAT4
	{
		Vector4(){}
		Vector4(float xyz)
			: XMFLOAT4(xyz, xyz, xyz, 1.0f)
		{
		}
		Vector4(float x, float y, float z, float w)
			: XMFLOAT4(x, y, z, w)
		{
		}
		Vector4(const XMFLOAT4& xyzw)
			: XMFLOAT4(xyzw)
		{
		}
		Vector4(const XMFLOAT3& xyz)
		{
			x = xyz.x;
			y = xyz.y;
			z = xyz.z;
			w = 1.0f;
		}
		Vector4(FXMVECTOR v)
		{
			XMStoreFloat4(this, v);
		}

		void Transform(const Matrix4x4& m);

		FXMVECTOR ToSIMD() const { return XMLoadFloat4(this); }

		float* ToFloatArray() { return reinterpret_cast<float*>(this); }
		const float* ToFloatArray() const { return reinterpret_cast<const float*>(this); }

	public:
		static Vector4 Transform(const Vector4& v, const Matrix4x4& m);
	};
	typedef Vector4 float4;


	/**
	 * Matrix4x4
	 */
	struct Matrix4x4 : public XMFLOAT4X4
	{
		Matrix4x4()
		{
		}
		Matrix4x4(const XMFLOAT4X4& m)
			: XMFLOAT4X4(m)
		{
		}
		Matrix4x4(CXMMATRIX m)
		{
			XMStoreFloat4x4(this, m);
		}

		Matrix4x4& operator*=(const Matrix4x4& other)
		{
			auto result = ToSIMD() * other.ToSIMD();
			XMStoreFloat4x4(this, result);
			return *this;
		}
		Matrix4x4 operator*(const Matrix4x4& other) const
		{
			auto result = ToSIMD() * other.ToSIMD();
			return Matrix4x4(result);
		}

		bool operator==(const Matrix4x4& other) const
		{
			return _11 == other._11 && _12 == other._12 && _13 == other._13 && _14 == other._14
				&& _21 == other._21 && _22 == other._22 && _23 == other._23 && _24 == other._24
				&& _31 == other._31 && _32 == other._32 && _33 == other._33 && _34 == other._34
				&& _41 == other._41 && _42 == other._42 && _43 == other._43 && _44 == other._44;
		}
		bool operator!=(const Matrix4x4& other) const
		{
			return !(*this == other);
		}

		void Ident()
		{
			_11 = _22 = _33 = _44 = 1.00f;
			_12 = _13 = _14 = 0.0f;
			_21 = _23 = _24 = 0.0f;
			_31 = _32 = _34 = 0.0f;
			_41 = _42 = _43 = 0.0f;
		}

		Vector3 Translation() const
		{
			return Vector3(_41, _42, _43);
		}

		void SetTranslation(const Vector3& t)
		{
			_41 = t.x;
			_42 = t.y;
			_43 = t.z;
		}

		XMMATRIX ToSIMD() const { return XMLoadFloat4x4(this); }

		/* static methods */
		static Matrix4x4 Transpose(const Matrix4x4& m)
		{
			return XMMatrixTranspose(m.ToSIMD());
		}

		static Matrix4x4 Invert(const Matrix4x4& m)
		{
			XMVECTOR det;
			return XMMatrixInverse(&det, m.ToSIMD());
		}

		static Matrix4x4 ScaleMatrix(float s)
		{
			Matrix4x4 m;
			m.Ident();
			m._11 = s;
			m._22 = s;
			m._33 = s;
			return m;
		}

		static Matrix4x4 ScaleMatrix(const Vector3& s)
		{
			Matrix4x4 m;
			m.Ident();
			m._11 = s.x;
			m._22 = s.y;
			m._33 = s.z;
			return m;
		}

		static Matrix4x4 TranslationMatrix(const Vector3& t)
		{
			Matrix4x4 m;
			m.Ident();
			m._41 = t.x;
			m._42 = t.y;
			m._43 = t.z;
			return m;
		}
	};
	typedef Matrix4x4 float4x4;


	/**
	 * Quaternion
	 */
	struct Quaternion
	{
		float x, y, z, w;

		Quaternion() {}

		Quaternion(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w) {}

		Quaternion(const float3& axis, float angle)
		{
			*this = SetFromAxisAngle(axis, angle);
		}

		Quaternion(const XMFLOAT4& q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
		}

		Quaternion(FXMVECTOR q)
		{
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), q);
		}

		Quaternion& operator*=(const Quaternion& other)
		{
			XMVECTOR q = ToSIMD();
			q = XMQuaternionMultiply(q, other.ToSIMD());
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), q);

			return *this;
		}

		Quaternion operator*(const Quaternion& other) const
		{
			Quaternion q = *this;
			q *= other;
			return q;
		}

		bool operator==(const Quaternion& other) const
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		bool operator!=(const Quaternion& other) const
		{
			return x != other.x || y != other.y || z != other.z || w != other.w;
		}

		float4x4 ToFloat4x4() const
		{
			return float4x4(XMMatrixRotationQuaternion(ToSIMD()));
		}

		Quaternion Identity()
		{
			return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		}

		Quaternion Invert(const Quaternion& q)
		{
			return Quaternion(XMQuaternionInverse(q.ToSIMD()));
		}

		Quaternion SetFromAxisAngle(const float3& axis, float angle)
		{
			XMVECTOR q = XMQuaternionRotationAxis(axis.ToSIMD(), angle);
			return Quaternion(q);
		}

		Quaternion SetFromEuler(float x, float y, float z)
		{
			XMVECTOR q = XMQuaternionRotationRollPitchYaw(x, y, z);
			return Quaternion(q);
		}

		Quaternion Normalize(const Quaternion& q)
		{
			return Quaternion(XMQuaternionNormalize(q.ToSIMD()));
		}

		float4x4 ToFloat4x4(const Quaternion& q)
		{
			return q.ToFloat4x4();
		}

		XMVECTOR ToSIMD() const
		{
			return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(this));
		}
	};


	/**
	 * Rect
	 */
	template <typename T>
	struct TRect
	{
		T x;
		T y;
		T width;
		T height;

		TRect()
		{
		}
		TRect(T x, T y, T width, T height)
			: x(x)
			, y(y)
			, width(width)
			, height(height)
		{
		}

		void Set(T x, T y, T width, T height)
		{
			this->x = x;
			this->y = y;
			this->width = width;
			this->height = height;
		}
	};
	typedef TRect<int32_t> Rect;
	typedef TRect<float> Rectf;


	template<typename T>
	__forceinline T Min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	__forceinline T Max(T a, T b)
	{
		return (a < b) ? b : a;
	}

	template<typename T> 
	__forceinline T Clamp(T val, T min, T max)
	{
		if (val < min) val = min;
		else if (val > max) val = max;
		return val;
	}

	__forceinline float DegreeToRadian(float deg)
	{
		return deg * (1.0f / 180.0f) * 3.14159265359f;
	}

	__forceinline float RadianToDegree(float rad)
	{
		return rad * (1.0f / 3.14159265359f) * 180.0f;
	}


	/** class inline functions **/
	__forceinline void Vector3::Transform(const Matrix4x4& m)
	{
		XMVECTOR vec = ToSIMD();
		*this = XMVector3TransformCoord(vec, m.ToSIMD());
	}

	__forceinline void Vector3::Transform(const Quaternion& q)
	{
		Transform(q.ToFloat4x4());
	}

	__forceinline Vector3 Vector3::Transform(const Vector3& v, const Matrix4x4& m)
	{
		XMVECTOR vec = v.ToSIMD();
		vec = XMVector3TransformCoord(vec, m.ToSIMD());
		return Vector3(vec);
	}

	__forceinline Vector3 Vector3::Transform(const Vector3& v, const Quaternion& q)
	{
		return Vector3::Transform(v, q.ToFloat4x4());
	}

	__forceinline Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
	{
		Vector3 result;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result), XMVector3Cross(a.ToSIMD(), b.ToSIMD()));
		return result;
	}


	__forceinline void Vector4::Transform(const float4x4& m)
	{
		XMVECTOR vec = ToSIMD();
		*this = XMVector4Transform(vec, m.ToSIMD());
	}

	__forceinline Vector4 Vector4::Transform(const Vector4& v, const Matrix4x4& m)
	{
		XMVECTOR vec = v.ToSIMD();
		vec = XMVector4Transform(vec, m.ToSIMD());
		return Vector4(vec);
	}
}