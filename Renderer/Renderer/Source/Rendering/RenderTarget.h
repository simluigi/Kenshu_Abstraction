#pragma once

#include "RenderCommon.h"
#include "../Utility/Math/Vector.h"

class RenderTarget
{
public:

	RenderTarget(const char* resourceName, const Int2& resourceSize, ResourceFormat format);
	virtual ~RenderTarget();

	ResourceFormat GetResourceFormat()const;

protected:

	Int2 m_resourceSize;
	ResourceFormat m_format;
	const char* m_resourceName = nullptr;
};

