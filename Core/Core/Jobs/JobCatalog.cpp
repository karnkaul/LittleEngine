#include "stdafx.h"
#include "JobManager.h"
#include "JobCatalog.h"
#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Core
{
JobCatalog::SubJob::SubJob(String name, Task job) : name(std::move(name)), job(std::move(job))
{
}

JobCatalog::JobCatalog(JobManager& manager, String name)
	: m_logName("[" + std::move(name) + "]"), m_pManager(&manager)
{
}

void JobCatalog::AddJob(Task job, String name)
{
	String n = name;
	if (name.empty())
	{
		n = "JobCatalog_" + Strings::ToString(m_subJobs.size());
	}
	m_subJobs.emplace_back(n, std::move(job));
}

void JobCatalog::StartJobs(Task onComplete)
{
	LOG_D("%s started. Running and monitoring %d jobs", LogNameStr(), m_subJobs.size());
	m_onComplete = onComplete;
	m_bCompleted = false;
	m_startTime = Time::Now();
	for (auto& job : m_subJobs)
	{
		m_pendingJobs.push_back(m_pManager->Enqueue(job.job, job.name));
	}
}

Fixed JobCatalog::GetProgress() const
{
	u32 done = m_subJobs.size() - m_pendingJobs.size();
	return Fixed(done, m_subJobs.size());
}

void JobCatalog::Update()
{
	auto iter = m_pendingJobs.begin();
	while (iter != m_pendingJobs.end())
	{
		if ((*iter)->HasCompleted())
		{
#if ENABLED(DEBUG_LOGGING)
			s32 id = (*iter)->GetID();
#endif
			iter = m_pendingJobs.erase(iter);
			LOG_D("%s Job %d completed. %d jobs remaining", LogNameStr(), id, m_pendingJobs.size());
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
#if ENABLED(DEBUG_LOGGING)
		f32 secs = (Time::Now() - m_startTime).AsSeconds();
#endif
		LOG_D("%s completed %d jobs in %.2fs", LogNameStr(), m_subJobs.size(), secs);
	}
}

const char* JobCatalog::LogNameStr() const
{
	return m_logName.c_str();
}
} // namespace Core