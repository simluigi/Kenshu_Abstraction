#include "Texture.h"

#include "RenderContext.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::SetTexture(RenderContext& renderContext, unsigned int shaderSlot)
{
	SetTexture(renderContext.GetMainCommandBuffer(), shaderSlot);
}
