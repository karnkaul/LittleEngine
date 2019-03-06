#pragma once
#include "CoreTypes.h"
#include "JobHandle.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
class MultiJob
{
private:
	struct SubJob
	{
		String name;
		std::function<void()> job;

		SubJob(const String& name, const std::function<void()>& job);
	};

	String m_logName;
	Vec<SubJob> m_subJobs;
	List<JobHandle> m_pendingJobIDs;
	List<JobHandle> m_completedJobIDs;
	std::function<void()> m_OnComplete = nullptr;
	Time m_startTime;
	bool m_bCompleted = false;

public:
	MultiJob(const String& name);

	void AddJob(const std::function<void()>& job, const String& name = "");
	void StartJobs(const std::function<void()>& OnComplete);
	Fixed GetProgress() const;

private:
	void Update();
	const char* LogNameStr() const;

	friend class JobManager;
};
} // namespace LittleEngine
