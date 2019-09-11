#pragma once
#include "Core/CoreTypes.h"

namespace Core
{
class JobHandleBlock final
{
private:
	Deferred<void> m_deferred;
	s64 m_jobID = -1;

public:
	JobHandleBlock() = default;
	JobHandleBlock(s64 jobID, Deferred<void>&& deferred);

	s64 ID() const;

	void Wait();
	bool HasCompleted() const;

private:
	friend class JobWorker;
	friend class JobManager;
};

using JobHandle = SPtr<JobHandleBlock>;
} // namespace Core
