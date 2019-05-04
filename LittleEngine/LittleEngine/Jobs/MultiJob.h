#pragma once
#include "CoreTypes.h"
#include "JobHandle.h"

namespace LittleEngine
{
class MultiJob
{
private:
	struct SubJob
	{
		String name;
		std::function<void()> job;

		SubJob(String name, std::function<void()> job);
	};

	String m_logName;
	Vec<SubJob> m_subJobs;
	List<JobHandle> m_pendingJobs;
	List<JobHandle> m_completedJobs;
	std::function<void()> m_onComplete = nullptr;
	Time m_startTime;
	bool m_bCompleted = false;

public:
	MultiJob(String name);

	void AddJob(std::function<void()> job, String name = "");
	void StartJobs(std::function<void()> onComplete);
	Fixed GetProgress() const;

private:
	void Update();
	const char* LogNameStr() const;

	friend class JobManager;
};
} // namespace LittleEngine
