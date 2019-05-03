#include "stdafx.h"
#include "JobHandle.h"

namespace LittleEngine
{
JobHandleBlock::JobHandleBlock(s64 jobID, std::future<void>&& future)
	: m_future(std::move(future)), m_jobID(jobID)
{
}

s64 JobHandleBlock::GetID() const
{
	return m_jobID;
}

void JobHandleBlock::Wait()
{
	if (m_future.valid())
	{
		m_future.get();
	}
}

bool JobHandleBlock::HasCompleted() const
{
	return m_future.valid() && m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}
} // namespace LittleEngine