#pragma once

#include <DirectXMath.h>
#include "../../Rendering/RenderObject.h"
#include "D3D11RenderResource.h"

class D3D11RenderObject :
	public RenderObject
{
public:

	D3D11RenderObject();
	~D3D11RenderObject();

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

	D3D11RenderResource* m_verticesResource = nullptr;
	D3D11RenderResource* m_indicesResource = nullptr;
	std::vector<D3D11RenderResource*> m_worldMatrixResources;
};
