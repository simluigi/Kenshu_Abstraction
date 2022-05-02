#pragma once

#include <memory>
#include <unordered_map>
#include "../Utility/Math/Vector.h"

class Window;
class Camera;
class RenderPass;
class RenderContext;
class RenderingData;

class Renderer
{
public:

	Renderer();
	virtual ~Renderer();

	virtual bool Create(const Window& window) = 0;
	virtual void Render() = 0;
	void Terminate();

	/**
	 * @return Camera CBV shader slot
	*/
	static unsigned int CameraShaderSlot();

	/**
	 * @return World matrix CBV slot
	*/
	static unsigned int WorldMatrixShaderSlot();

	/**
	 * @return Shadow camera CBV slot
	*/
	static unsigned int LightViewCameraShaderSlot();

	/**
	 * @brief Create render camera
	 * @param cameraName Name to register
	 * @param screenSize Render screen size
	 * @return Created camera
	*/
	virtual Camera& CreateCamera(const char* cameraName, const Int2& screenSize) = 0;

	/**
	 * @brief add renderPass
	*/
	void EnqueueRenderPass(RenderPass* renderPass);

protected:

	std::unique_ptr<RenderContext> m_renderContext;
	std::unique_ptr<RenderingData> m_renderingData;

	std::vector<RenderPass*> m_renderPasses;
};