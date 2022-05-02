
#include "ShaderCommon.hlsli"

struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	output.pos = float4(input.pos, 1);
	output.uv = input.uv;

	return output;
}