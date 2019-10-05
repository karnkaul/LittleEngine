#pragma once
#include <list>
#include <vector>
#include "JobHandle.h"

namespace Core
{
class JobCatalog
{
private:
	using Task = std::function<void()>;
	using SubJob = std::pair<std::string, Task>;

	std::string m_logName;
	std::vector<SubJob> m_subJobs;
	std::list<JobHandle> m_pendingJobs;
	std::list<JobHandle> m_completedJobs;
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
