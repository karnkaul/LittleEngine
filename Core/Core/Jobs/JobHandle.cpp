#include "stdafx.h"
#include "JobHandle.h"

namespace Core
{
JobHandleBlock::JobHandleBlock(s64 jobID, Deferred<void>&& deferred)
	: m_deferred(std::move(deferred)), m_jobID(jobID)
{
}

s64 JobHandleBlock::GetID() const
{
	return m_jobID;
}

void JobHandleBlock::Wait()
{
	m_deferred.SafeWait();
}

bool JobHandleBlock::HasCompleted() const
{
	return m_deferred.IsReady();
}
} // namespace Core