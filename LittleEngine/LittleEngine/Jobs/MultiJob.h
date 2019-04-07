#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"

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
	List<SPtr<class JobHandle>> m_pendingJobs;
	List<SPtr<JobHandle>> m_completedJobs;
	std::function<void()> m_OnComplete = nullptr;
	Time m_startTime;
	bool m_bCompleted = false;

public:
	MultiJob(String name);

	void AddJob(const std::function<void()>& job, String name = "");
	void StartJobs(const std::function<void()>& OnComplete);
	Fixed GetProgress() const;

private:
	void Update();
	const char* LogNameStr() const;

	friend class JobManager;
};
} // namespace LittleEngine
