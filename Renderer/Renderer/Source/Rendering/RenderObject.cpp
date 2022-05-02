#include "RenderObject.h"

#include "Texture.h"
#include "RenderContext.h"

RenderObject::RenderObject()
{
}

RenderObject::~RenderObject()
{
}

void RenderObject::AddTexture(std::unique_ptr<Texture> texture, unsigned int shaderSlot)
{
	m_textures.emplace_back(std::make_pair(std::move(texture), shaderSlot));
}

void RenderObject::DrawIndexed(RenderContext& renderContext, Transform& transform)
{
	DrawIndexed(renderContext.GetMainCommandBuffer(), transform);
}

void RenderObject::DrawIndexed(RenderContext& renderContext)
{
	DrawIndexed(renderContext.GetMainCommandBuffer());
}
