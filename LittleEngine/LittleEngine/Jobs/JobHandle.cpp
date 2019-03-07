#include "stdafx.h"
#include "JobHandle.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
JobHandle::JobHandle(s64 jobID, std::future<void>&& future)
	: m_future(std::move(future)), m_jobID(jobID)
{
}

s64 JobHandle::GetID() const
{
	return m_jobID;
}

void JobHandle::Wait()
{
	if (m_future.valid())
		m_future.get();
}

bool JobHandle::HasCompleted() const
{
	return m_future.valid() && m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}
} // namespace LittleEngine