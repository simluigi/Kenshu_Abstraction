#pragma once

#include <vector>
#include "../Utility/Math/Vector.h"

enum class ResourceFormat
{
	UNKNOWN,
	R16_UINT,
	R32_UINT,
	R8G8B8A8_UNORM,
	R32G32_FLOAT,
	R32G32B32_FLOAT,
	R32G32B32A32_FLOAT,
};

enum class ResourceType
{
	VertexBuffer,
	IndexBuffer,
	ShaderResource,
	ConstantBuffer,
	DepthBuffer,
};

struct MeshVertex
{
	Float3 pos;
	Float3 normal;
	Float2 uv;
};

/**
 * @brief Convert from ShaderSlot to RootParameterIndex
*/
struct RootParameterIndex
{
	std::vector<unsigned int> srvIndex;
	std::vector<unsigned int> cbvIndex;
	std::vector<unsigned int> uavIndex;
};