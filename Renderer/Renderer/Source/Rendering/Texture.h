#pragma once

class CommandBuffer;
class RenderContext;

class Texture
{
public:

	Texture();
	virtual ~Texture();

	/**
	 * @brief Set the texture to the shader (Use main command buffer)
	 * @param shaderSlot Shader register num
	*/
	void SetTexture(RenderContext& renderContext, unsigned int shaderSlot);

	/**
	 * @brief Set the texture to the shader
	 * @param shaderSlot Shader register num
	*/
	virtual void SetTexture(CommandBuffer& commandBuffer, unsigned int shaderSlot) = 0;

private:

};

