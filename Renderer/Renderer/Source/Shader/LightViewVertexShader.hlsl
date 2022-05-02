#include "ShaderCommon.hlsli"

cbuffer CameraInfo : register(b0)
{
	matrix view;
	matrix projection;
	float4 cameraPos;
};

cbuffer WorldMatrixCBuffer : register(b1)
{
	matrix WorldMatrix;
};

float4 main(VSInput input) : SV_POSITION
{
	float4 worldPosition = mul(float4(input.pos, 1.0f), WorldMatrix);

	return mul(mul(worldPosition, view), projection);
}