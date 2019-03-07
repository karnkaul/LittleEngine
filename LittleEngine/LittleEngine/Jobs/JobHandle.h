#pragma once
#include <future>
#include "CoreTypes.h"

namespace LittleEngine
{
class JobHandle final
{
private:
	std::future<void> m_future;
	s64 m_jobID = -1;

public:
	JobHandle() = default;
	JobHandle(s64 jobID, std::future<void>&& future);

	s64 GetID() const;

	void Wait();
	bool HasCompleted() const;

private:
	friend class JobWorker;
	friend class JobManager;
};
} // namespace LittleEngine