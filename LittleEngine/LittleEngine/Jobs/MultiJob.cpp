#include "stdafx.h"
#include "Logger.h"
#include "Utils.h"
#include "JobManager.h"
#include "JobHandle.h"
#include "MultiJob.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
MultiJob::SubJob::SubJob(String name, std::function<void()> job)
	: name(std::move(name)), job(std::move(job))
{
}

MultiJob::MultiJob(String name) : m_logName("[" + std::move(name) + " (MultiJob)]")
{
}

void MultiJob::AddJob(const std::function<void()>& job, String name)
{
	String n = name;
	if (name.empty())
	{
		n = "MultiJob_" + Strings::ToString(m_subJobs.size());
	}
	m_subJobs.emplace_back(n, job);
}

void MultiJob::StartJobs(const std::function<void()>& OnComplete)
{
	LOG_I("%s started. Running and monitoring %d jobs", LogNameStr(), m_pendingJobs.size());
	JobManager* pJobs = Services::Jobs();
	m_OnComplete = OnComplete;
	m_bCompleted = false;
	m_startTime = Time::Now();
	for (auto& job : m_subJobs)
	{
		m_pendingJobs.push_back(pJobs->Enqueue(job.job, job.name));
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
		if (m_OnComplete)
		{
			m_OnComplete();
			m_OnComplete = nullptr;
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