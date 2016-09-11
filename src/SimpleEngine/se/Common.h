#pragma once


// �v���b�g�t�H�[���w�b�_
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <stdint.h>
#include <assert.h>
#include <array>
#include <algorithm>

// �}�N��
#define arraySize(p)		(sizeof (p) /sizeof p[0])

#ifdef _DEBUG

	// �f�o�b�O�o�͗p
	inline void DebugPrintf(const char *fmt, ...)
	{
		char buf[1024];
		va_list list;
		va_start(list, fmt);
		vsnprintf(buf, ARRAYSIZE(buf) - 1, fmt, list);
		va_end(list);
		buf[ARRAYSIZE(buf) - 1] = '\0';
		OutputDebugStringA(buf);
	}

	#define Printf(...)			DebugPrintf(__VA_ARGS__)
	//#define Printf(...)			printf(__VA_ARGS__)
	#define Assert(expr)		assert(expr)

#else

	#define Assert(expr)
	#define Printf(...)

#endif
