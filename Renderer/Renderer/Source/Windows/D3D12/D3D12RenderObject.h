#pragma once

#include <vector>
#include <DirectXMath.h>
#include "../../Rendering/RenderObject.h"

class RenderingData;
class D3D12RenderResource;

class D3D12RenderObject :
	public RenderObject
{
public:

	D3D12RenderObject();
	~D3D12RenderObject();

	/**
	 * @brief Generate mesh
	 * @param vertices mesh vertices
	 * @param indices mesh indices
	*/
	void Create(RenderingData& renderingData, const char* objectName, std::vector<MeshVertex>& vertices, std::vector<uint16_t>& indices)override;

	void DrawIndexed(CommandBuffer& commandBuffer)override;
	void DrawIndexed(CommandBuffer& commandBuffer, Transform& transform, bool setTexture = true)override;

private:

	DirectX::XMFLOAT4X4 GetWorldMatrix(Transform& transform);

	D3D12RenderResource* m_verticesResource = nullptr;
	D3D12RenderResource* m_indicesResource = nullptr;
	std::vector<D3D12RenderResource*> m_worldMatrixResources;
};