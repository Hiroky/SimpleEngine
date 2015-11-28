#pragma once 

#include "se/Common.h"

namespace se
{
	template <class T>
	class Vector2Base
	{
	public:
		union
		{
			struct
			{
				T x, y;
			};
			T v[2];
		};

		Vector2Base() {}
		Vector2Base(T x, T y)
			: x(x), y(y)
		{
		}
	};

	template <class T>
	class Vector3Base
	{
	public:
		union
		{
			struct
			{
				T x, y, z;
			};
			T v[3];
		};

		Vector3Base() {}
		Vector3Base(T x, T y, T z)
			: x(x), y(y), z(z)
		{
		}
	};

	template <class T>
	class Vector4Base
	{
	public:
		union
		{
			struct
			{
				T x, y, z, w;
			};
			T v[4];
		};

		Vector4Base() {}
		Vector4Base(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w)
		{
		}
	};

	typedef Vector2Base<float> Vector2;
	typedef Vector2Base<float> float2;
	typedef Vector3Base<float> Vector3;
	typedef Vector3Base<float> float3;
	typedef Vector4Base<float> Vector4;
	typedef Vector4Base<float> float4;
}