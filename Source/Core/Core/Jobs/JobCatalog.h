#pragma once
#include "JobHandle.h"

namespace Core
{
class JobCatalog
{
private:
	using SubJob = Pair<std::string, Task>;

	std::string m_logName;
	std::vector<SubJob> m_subJobs;
	List<JobHandle> m_pendingJobs;
	List<JobHandle> m_completedJobs;
	Task m_onComplete = nullptr;
	class JobManager* m_pManager;
	Time m_startTime;
	bool m_bCompleted = false;

public:
	JobCatalog(JobManager& manager, std::string name);

	void AddJob(Task job, std::string name = "");
	void StartJobs(Task onComplete);
	Fixed Progress() const;

private:
	void Update();

	friend class JobManager;
};
} // namespace Core
