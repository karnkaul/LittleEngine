#include "stdafx.h"
#include <thread>
#include "JobManager.h"
#include "JobWorker.h"
#include "MultiJob.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Core/OS.h"

namespace Core
{
JobManager::Job::Job() = default;

JobManager::Job::Job(s32 id, Task task, String name, bool bSilent)
	: m_task(std::move(task)), m_id(id), m_bSilent(bSilent)
{
	String suffix = name.empty() ? "" : "-" + name;
	logName = "[" + Strings::ToString(id) + suffix + "]";
	m_sHandle = MakeShared<JobHandleBlock>(id, m_promise.get_future());
}

const char* JobManager::Job::ToStr() const
{
	return logName.c_str();
}

void JobManager::Job::Run()
{
	try
	{
		m_task();
	}
	catch (const std::exception& e)
	{
		AssertVar(false, e.what());
		m_szException = e.what();
	}
}

void JobManager::Job::Fulfil()
{
	m_promise.set_value();
}

JobManager::JobManager(u32 workerCount)
{
	u32 maxThreads = OS::Threads::GetVacantThreadCount();
	workerCount = Maths::Min(workerCount, maxThreads);
	for (u32 i = 0; i < workerCount; ++i)
	{
		m_jobWorkers.emplace_back(MakeUnique<JobWorker>(*this, i + 100, false));
	}
	LOG_I("[JobManager] Detected [%d] max threads; spawned [%d] JobWorkers", maxThreads, workerCount);
}

JobManager::~JobManager()
{
	for (auto& worker : m_jobWorkers)
	{
		worker->Stop();
	}
	m_jobWorkers.clear();
	LOG_I("[JobManager] destroyed");
}

JobHandle JobManager::Enqueue(Task task, String name, bool bSilent)
{
	UPtr<Job> uJob = MakeUnique<Job>(++m_nextGameJobID, std::move(task), std::move(name), bSilent);
	return Lock_Enqueue(std::move(uJob), m_jobQueue);
}

MultiJob* JobManager::CreateMultiJob(String name)
{
	m_uMultiJobs.emplace_back(MakeUnique<MultiJob>(*this, std::move(name)));
	return m_uMultiJobs.back().get();
}

void JobManager::Update()
{
	auto iter = m_uMultiJobs.begin();
	while (iter != m_uMultiJobs.end())
	{
		(*iter)->Update();
		if ((*iter)->m_bCompleted)
		{
			LOG_D("[JobManager] %s completed. Destroying instance.", (*iter)->LogNameStr());
			iter = m_uMultiJobs.erase(iter);
			continue;
		}
		++iter;
	}
}

UPtr<JobManager::Job> JobManager::Lock_PopJob()
{
	Lock lock(m_mutex);
	if (!m_jobQueue.empty())
	{
		UPtr<Job> uJob = std::move(m_jobQueue.back());
		m_jobQueue.pop_back();
		return uJob;
	}
	return {};
}

JobHandle JobManager::Lock_Enqueue(UPtr<Job>&& uJob, List<UPtr<Job>>& jobQueue)
{
	JobHandle sHandle = uJob->m_sHandle;
	Lock lock(m_mutex);
	jobQueue.emplace_front(std::move(uJob));
	return sHandle;
}

bool JobManager::AreWorkersIdle()
{
	for (auto& gameWorker : m_jobWorkers)
	{
		if (gameWorker->GetState() == JobWorker::State::WORKING)
		{
			return false;
		}
	}
	Lock lock(m_mutex);
	return m_jobQueue.empty();
}
} // namespace Core
