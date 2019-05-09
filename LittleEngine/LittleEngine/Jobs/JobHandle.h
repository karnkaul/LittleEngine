#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class JobHandleBlock final
{
private:
	Deferred<void> m_deferred;
	s64 m_jobID = -1;

public:
	JobHandleBlock() = default;
	JobHandleBlock(s64 jobID, Deferred<void>&& future);

	s64 GetID() const;

	void Wait();
	bool HasCompleted() const;

private:
	friend class JobWorker;
	friend class JobManager;
};

using JobHandle = SPtr<JobHandleBlock>;
} // namespace LittleEngine