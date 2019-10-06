#include "../LECoreUtils/LELogger/Logger.h"
#include "../LECoreUtils/Utils.h"
#include "JobManager.h"
#include "JobCatalog.h"

namespace Core
{
JobCatalog::JobCatalog(JobManager& manager, std::string name) : m_pManager(&manager) 
{
	m_logName.reserve(name.size() + 2);
	m_logName = "[";
	m_logName += std::move(name);
	m_logName += "]";
}

void JobCatalog::AddJob(Task job, std::string name)
{
	if (name.empty())
	{
		name = "JobCatalog_" + Strings::ToString(m_subJobs.size());
	}
	m_subJobs.emplace_back(std::move(name), std::move(job));
}

void JobCatalog::StartJobs(Task onComplete)
{
	LOG_D("%s started. Running and monitoring %d jobs", m_logName.c_str(), m_subJobs.size());
	m_onComplete = onComplete;
	m_bCompleted = false;
	m_startTime = Time::Now();
	for (auto& job : m_subJobs)
	{
		m_pendingJobs.push_back(m_pManager->Enqueue(std::move(job.second), std::move(job.first)));
	}
}

Fixed JobCatalog::Progress() const
{
	u32 done = static_cast<u32>(m_subJobs.size() - m_pendingJobs.size());
	return Fixed(ToS32(done), ToS32(m_subJobs.size()));
}

void JobCatalog::Update()
{
	auto iter = m_pendingJobs.begin();
	while (iter != m_pendingJobs.end())
	{
		const auto& subJob = *iter;
		if (subJob->HasCompleted())
		{
#if defined(DEBUG_LOGGING) && DEBUG_LOGGING
			auto id = subJob->ID();
#endif
			iter = m_pendingJobs.erase(iter);
			LOG_D("%s Job %d completed. %d jobs remaining", m_logName.c_str(), id, m_pendingJobs.size());
		}
		else
		{
			++iter;
		}
	}

	if (m_pendingJobs.empty() && !m_bCompleted)
	{
		if (m_onComplete)
		{
			m_onComplete();
			m_onComplete = nullptr;
		}
		m_bCompleted = true;
#if defined(DEBUG_LOGGING) && DEBUG_LOGGING
		f32 secs = (Time::Now() - m_startTime).AsSeconds();
		LOG_D("%s completed %d jobs in %.2fs", m_logName.c_str(), m_subJobs.size(), secs);
#endif
	}
}
} // namespace Core
