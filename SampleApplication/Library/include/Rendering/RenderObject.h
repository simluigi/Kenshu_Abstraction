#pragma once

#include <vector>
#include <memory>
#include "../Utility/Math/Transform.h"
#include "../Utility/Math/Vector.h"
#include "RenderCommon.h"

class Texture;
class RenderingData;
class CommandBuffer;
class RenderContext;
struct MeshVertex;

class RenderObject
{
public:

	RenderObject();
	virtual ~RenderObject();

	/**
	 * @brief Generate mesh
	 * @param vertices mesh vertices
	 * @param indices mesh indices
	*/
	virtual void Create(RenderingData& renderingData, const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices) = 0;

	/**
	 * @brief Draw indexed (Use main command buffer)
	*/
	void DrawIndexed(RenderContext& renderContext, Transform& transform);

	virtual void DrawIndexed(CommandBuffer& commandBuffer, Transform& transform, bool setTexture = true) = 0;

	void DrawIndexed(RenderContext& renderContext);
	virtual void DrawIndexed(CommandBuffer& commandBuffer) = 0;

	/**
	 * @brief Set the texture to the shader
	 * @param texture used texture
	 * @param shaderSlot shader register num
	*/
	virtual void AddTexture(std::unique_ptr<Texture> texture, unsigned int shaderSlot);

protected:

	const char* m_objectName = "";

	std::vector<MeshVertex> m_vertices; /// meshVertices
	std::vector<uint16_t> m_indices; /// meshIndices

	std::vector<std::pair<std::unique_ptr<Texture>, unsigned int>> m_textures; /// pair (texture, shaderSlot)
};

