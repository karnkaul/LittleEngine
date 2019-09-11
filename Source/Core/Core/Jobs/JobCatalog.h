#pragma once
#include "JobHandle.h"

namespace Core
{
class JobCatalog
{
private:
	using SubJob = Pair<String, Task>;

	String m_logName;
	Vec<SubJob> m_subJobs;
	List<JobHandle> m_pendingJobs;
	List<JobHandle> m_completedJobs;
	Task m_onComplete = nullptr;
	class JobManager* m_pManager;
	Time m_startTime;
	bool m_bCompleted = false;

public:
	JobCatalog(JobManager& manager, String name);

	void AddJob(Task job, String name = "");
	void StartJobs(Task onComplete);
	Fixed Progress() const;

private:
	void Update();
	const char* LogNameStr() const;

	friend class JobManager;
};
} // namespace Core
