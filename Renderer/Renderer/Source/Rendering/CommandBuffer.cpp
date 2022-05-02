#include "CommandBuffer.h"

CommandBuffer::CommandBuffer()
{
}

CommandBuffer::~CommandBuffer()
{
}

void CommandBuffer::SetFrameResourceIndex(unsigned int frameResourceIndex)
{
	m_frameResourceIndex = frameResourceIndex;
}

unsigned int CommandBuffer::GetFrameResourceIndex() const
{
	return m_frameResourceIndex;
}
