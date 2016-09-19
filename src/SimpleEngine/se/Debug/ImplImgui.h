#pragma once 

#include "se/Graphics/Graphics.h"
#include "thirdparty/imgui/imgui.h"


namespace se
{
	/**
	 * functions
	 */
	bool ImGuiInitialize();
	void ImGuiShutdown();
	void ImGuiNewFrame();
	void ImGuiRender();
	void ImGuiAddFontFromFileTTF(const char* filename, float sizePixels);


	/** 
	 * ImVec2 operators override
	 */
	struct IMVec2 : public ImVec2
	{
		IMVec2(float x, float y)
			: ImVec2(x, y)
		{
		}

		__forceinline IMVec2 operator +(const ImVec2& a)
		{
			return IMVec2(x + a.x, y + a.y);
		}
		__forceinline IMVec2 operator -(const ImVec2& a)
		{
			return IMVec2(x - a.x, y - a.y);
		}
		__forceinline IMVec2 operator -()
		{
			return IMVec2(-x, -y);
		}
		__forceinline IMVec2 operator *(const ImVec2& a)
		{
			return IMVec2(x * a.x, y * a.y);
		}
		__forceinline IMVec2 operator /(const ImVec2& a)
		{
			return IMVec2(x / a.x, y / a.y);
		}
		__forceinline IMVec2 operator *(float a)
		{
			return IMVec2(x * a, y * a);
		}
		__forceinline IMVec2 operator /(float a)
		{
			return *this * (1.0f / a);
		}
		__forceinline const IMVec2& operator *=(const ImVec2& a)
		{
			x *= a.x;
			y *= a.y;
			return *this;
		}
		__forceinline const IMVec2& operator /=(const ImVec2& a)
		{
			x /= a.x;
			y /= a.y;
			return *this;
		}
		__forceinline const IMVec2& operator *=(float a)
		{
			x *= a;
			y *= a;
			return *this;
		}
		__forceinline const IMVec2& operator /=(float a)
		{
			*this *= 1.0f / a;
			return *this;
		}
	};
}