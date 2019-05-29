#include "stdafx.h"
#include "JobManager.h"
#include "MultiJob.h"
#include "Core/Logger.h"
#include "Core/Utils.h"

namespace Core
{
MultiJob::SubJob::SubJob(String name, std::function<void()> job)
	: name(std::move(name)), job(std::move(job))
{
}

MultiJob::MultiJob(JobManager& manager, String name) : m_logName("[" + std::move(name) + "]"), m_pManager(&manager)
{
}

void MultiJob::AddJob(std::function<void()> job, String name)
{
	String n = name;
	if (name.empty())
	{
		n = "MultiJob_" + Strings::ToString(m_subJobs.size());
	}
	m_subJobs.emplace_back(n, std::move(job));
}

void MultiJob::StartJobs(std::function<void()> onComplete)
{
	LOG_I("%s started. Running and monitoring %d jobs", LogNameStr(), m_subJobs.size());
	m_onComplete = onComplete;
	m_bCompleted = false;
	m_startTime = Time::Now();
	for (auto& job : m_subJobs)
	{
		m_pendingJobs.push_back(m_pManager->Enqueue(job.job, job.name));
	}
}

Fixed MultiJob::GetProgress() const
{
	u32 done = m_subJobs.size() - m_pendingJobs.size();
	return Fixed(done, m_subJobs.size());
}

void MultiJob::Update()
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
		f32 secs = (Time::Now() - m_startTime).AsSeconds();
		LOG_I("%s completed %d jobs in %.2fs", LogNameStr(), m_subJobs.size(), secs);
	}
}

const char* MultiJob::LogNameStr() const
{
	return m_logName.c_str();
}
} // namespace LittleEngine