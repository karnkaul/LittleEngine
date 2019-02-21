#include "stdafx.h"
#include "Logger.h"
#include "Utils.h"
#include "MultiJob.h"
#include "JobManager.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
MultiJob::SubJob::SubJob(const String& name, Function(void()) job) : name(name), job(job)
{
}

MultiJob::MultiJob(const String& name) : m_logName("[" + name + " (MultiJob)]")
{
}

void MultiJob::AddJob(Function(void()) job, const String& name)
{
	String n = name;
	if (name.empty())
		n = "MultiJob_" + Strings::ToString(m_subJobs.size());
	m_subJobs.emplace_back(n, job);
}

void MultiJob::StartJobs(Function(void()) OnComplete)
{
	JobManager* pJobs = Services::Jobs();
	m_OnComplete = OnComplete;
	m_bCompleted = false;
	for (auto& job : m_subJobs)
	{
		m_pendingJobIDs.push_back(pJobs->Enqueue(job.job, job.name));
	}
	LOG_I("%s started. Running and monitoring %d jobs", LogNameStr(), m_pendingJobIDs.size());
}

Fixed MultiJob::GetProgress() const
{
	u32 done = m_subJobs.size() - m_pendingJobIDs.size();
	return Fixed(done, m_subJobs.size());
}

void MultiJob::Update()
{
	auto iter = m_pendingJobIDs.begin();
	while (iter != m_pendingJobIDs.end())
	{
		if (Services::Jobs()->IsCompleted(*iter))
		{
#if ENABLED(DEBUG_LOGGING)
			JobID id = *iter;
#endif
			iter = m_pendingJobIDs.erase(iter);
			LOG_D("%s Job %d completed. %d jobs remaining", LogNameStr(), id, m_pendingJobIDs.size());
		}
		else
		{
			++iter;
		}
	}

	if (m_pendingJobIDs.empty() && m_OnComplete)
	{
		m_OnComplete();
		m_OnComplete = nullptr;
		m_bCompleted = true;
	}
}

const char* MultiJob::LogNameStr() const
{
	return m_logName.c_str();
}
} // namespace LittleEngine