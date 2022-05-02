#include "RenderTarget.h"

RenderTarget::RenderTarget(const char* resourceName, const Int2& resourceSize, ResourceFormat format) :
	m_resourceName(resourceName), m_resourceSize(resourceSize), m_format(format)
{
}

RenderTarget::~RenderTarget()
{
}

ResourceFormat RenderTarget::GetResourceFormat() const
{
	return m_format;
}
