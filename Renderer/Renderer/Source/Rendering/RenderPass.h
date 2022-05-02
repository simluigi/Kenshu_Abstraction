#pragma once

class Camera;
class RenderContext;
class RenderingData;

class RenderPass
{
public:

	RenderPass();
	virtual ~RenderPass();

	virtual bool Create(RenderContext& renderContext, RenderingData& renderingData) = 0;
	virtual void Render(RenderContext& renderContext, RenderingData& renderingData) = 0;
	virtual void Terminate(RenderContext& renderContext) = 0;

private:

};