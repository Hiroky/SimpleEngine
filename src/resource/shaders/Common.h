#ifndef COMMON_H
#define COMMON_H

/**
 * ビューパラメータ
 */
struct ViewParameterData
{
	float4x4 	worldToView;
	float4x4	viewToClip;
	float4x4 	worldToClip;
};

/**
 * オブジェクトパラメータ
 */
struct ObjectParameterData
{
	float4x4 localToWorld;
};

#endif
