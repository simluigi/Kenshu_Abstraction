#pragma once

#include "RenderCommon.h"

class RenderResource
{
public:

	RenderResource();
	RenderResource(const char* resourceName, ResourceType resourceType, const void* resourceData,
		unsigned int dataCount, unsigned int stride, ResourceFormat format = ResourceFormat::UNKNOWN);

	virtual ~RenderResource();

	const ResourceType GetResourceType()const;
	const ResourceFormat GetDataFormat()const;

	const unsigned int GetDataCount()const;
	const unsigned int GetDataStride()const;

protected:

	const char* m_resourceName = "";

	ResourceType m_resourceType = {};
	ResourceFormat m_dataFormat = {};

	const void* m_resourceData = nullptr;
	unsigned int m_dataSize = 0;
	unsigned int m_dataStride = 0;
	unsigned int m_dataCount = 0;
};