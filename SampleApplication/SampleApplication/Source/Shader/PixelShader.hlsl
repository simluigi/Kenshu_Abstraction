
#include "ShaderCommon.hlsli"

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 worldPosition : WORLDPOSITION;
	float4 cameraPosition : CAMERAPOSITION;
	float4 lightViewPosition : TEXCOORD1;
	float4 normal : NORMAL;
	float2 uv : TEXCOORD;
};

Texture2D MeshTexture : register(t0);
Texture2D LightViewDepthTexture : register(t1);
SamplerState Sampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
	float3 lightDirection = normalize(float3(-0.2f, -0.3, 0.3));

	float3 normal = normalize(input.normal.xyz);

	float diffuse = saturate(dot(-lightDirection, normal));

	float3 viewDirection = normalize(input.cameraPosition.xyz - input.worldPosition.xyz);
	float3 halfVec = normalize(-lightDirection + viewDirection);

	float specularPower = 8;
	float3 specular = pow(max(dot(normal, halfVec), 0), specularPower);

	float4 texColor = MeshTexture.Sample(Sampler, input.uv);

	float4 color = float4((texColor.rgb * diffuse) + specular, texColor.a);

	float3 lightViewPosition = input.lightViewPosition.xyz / input.lightViewPosition.w;

	float2 shadowUV = lightViewPosition.xy;
	shadowUV *= float2(0.5f, -0.5f);
	shadowUV += 0.5f;

	if (shadowUV.x > 0.0f && shadowUV.x < 1.0f
		&& shadowUV.y > 0.0f && shadowUV.y < 1.0f)
	{
		float shadow = LightViewDepthTexture.Sample(Sampler, shadowUV).r;

		if (lightViewPosition.z - 0.0001f > shadow)
		{
			color.xyz *= 0.3f;
		}
	}

	return color;
}