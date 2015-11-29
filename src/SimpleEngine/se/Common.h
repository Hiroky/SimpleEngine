#pragma once


// プラットフォームヘッダ
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <array>
#include <algorithm>

// 型
typedef unsigned int				uint;
typedef unsigned short				ushort;

// マクロ
#define arraySize(p)		(sizeof (p) /sizeof p[0])
