
struct PSInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D MainTexture : register(t0);
SamplerState Sampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
	float4 color = MainTexture.Sample(Sampler, input.uv);

	return color;
}