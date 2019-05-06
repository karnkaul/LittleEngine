#pragma once
#include <future>
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class JobHandleBlock final
{
private:
	std::future<void> m_future;
	s64 m_jobID = -1;

public:
	JobHandleBlock() = default;
	JobHandleBlock(s64 jobID, std::future<void>&& future);

	s64 GetID() const;

	void Wait();
	bool HasCompleted() const;

private:
	friend class JobWorker;
	friend class JobManager;
};

using JobHandle = SPtr<JobHandleBlock>;
} // namespace LittleEngine