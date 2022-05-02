
#include "ShaderCommon.hlsli"

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 worldPosition : WORLDPOSITION;
	float4 cameraPosition : CAMERAPOSITION;
	float4 lightViewPosition : TEXCOORD1;
	float4 normal : NORMAL;
	float2 uv : TEXCOORD;
};

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

cbuffer LightViewCameraInfo : register(b2)
{
	matrix lightView;
	matrix lightProjection;
	float4 lightCameraPos;
};

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	output.worldPosition = mul(float4(input.pos, 1.0f), WorldMatrix);
	output.pos = mul(output.worldPosition, view);
	output.pos = mul(output.pos, projection);
	output.cameraPosition = cameraPos;

	output.lightViewPosition = mul(output.worldPosition, lightView);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjection);

	output.uv = input.uv;
	output.normal = mul(float4(input.normal, 0.0f), WorldMatrix);

	return output;
}