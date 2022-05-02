#include "RenderResource.h"

RenderResource::RenderResource()
{
}

RenderResource::RenderResource(const char* resourceName, ResourceType resourceType, const void* resourceData, unsigned int dataCount, unsigned int stride, ResourceFormat format) :
	m_resourceName(resourceName), m_resourceType(resourceType), m_resourceData(resourceData), m_dataCount(dataCount), m_dataStride(stride), m_dataSize(stride* dataCount), m_dataFormat(format)
{
}

RenderResource::~RenderResource()
{
}

const ResourceType RenderResource::GetResourceType() const
{
	return m_resourceType;
}

const ResourceFormat RenderResource::GetDataFormat() const
{
	return m_dataFormat;
}

const unsigned int RenderResource::GetDataCount() const
{
	return m_dataCount;
}

const unsigned int RenderResource::GetDataStride() const
{
	return m_dataStride;
}
