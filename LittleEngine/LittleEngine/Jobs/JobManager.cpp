#include "stdafx.h"
#include <thread>
#include "Logger.h"
#include "Utils.h"
#include "JobHandle.h"
#include "JobManager.h"
#include "JobWorker.h"
#include "MultiJob.h"
#include "LittleEngine/Engine/OS.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
JobManager::Job::Job(s32 id, std::function<void()> task, String name, bool bSilent)
	: m_task(std::move(task)), m_id(id), m_bSilent(bSilent)
{
	String suffix = name.empty() ? "" : "-" + name;
	logName = "[" + Strings::ToString(id) + suffix + "]";
	m_sHandle = MakeShared<JobHandle>(id, m_promise.get_future());
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
		Assert(false, "AsyncTask threw exception!");
		LOG_E("%d threw an exception and job has failed! %d\n%s", m_sHandle->GetID(), e.what());
	}
}

void JobManager::Job::Fulfil()
{
	m_promise.set_value();
}

JobManager::JobManager()
{
	u32 engineWorkers = OS::Platform()->SystemWorkerCount();
	u32 gameWorkers = OS::Platform()->UserWorkerCount();
	m_availableEngineThreads = engineWorkers;
	for (u8 i = 0; i < gameWorkers; ++i)
	{
		m_gameWorkers.emplace_back(MakeUnique<JobWorker>(*this, i + 100, false));
	}
	for (u8 i = 0; i < engineWorkers; ++i)
	{
		m_engineWorkers.emplace_back(MakeUnique<JobWorker>(*this, i, true));
	}
	LOG_I(
		"[JobManager] Detected [%d] available threads. Spawned [%d] Engine JobWorkers and [%d] "
		"Game JobWorkers",
		OS::Platform()->TotalThreadCount(), engineWorkers, gameWorkers);
	Services::ProvideJobManager(*this);
}

JobManager::~JobManager()
{
	Services::UnprovideJobManager(*this);
	for (auto& worker : m_gameWorkers)
	{
		worker->Stop();
	}
	m_gameWorkers.clear();
	for (auto& worker : m_engineWorkers)
	{
		worker->Stop();
	}
	m_engineWorkers.clear();
	LOG_I("[JobManager] destroyed");
}

SPtr<JobHandle> JobManager::Enqueue(std::function<void()> task, String name, bool bSilent)
{
	UPtr<Job> uJob = MakeUnique<Job>(++m_nextGameJobID, std::move(task), std::move(name), bSilent);
	return Lock_Enqueue(std::move(uJob), m_gameJobQueue);
}

SPtr<JobHandle> JobManager::EnqueueEngine(std::function<void()> task, String name)
{
	Assert(AvailableEngineThreads() > 0, "!DEADLOCK! No available engine workers!");
	UPtr<Job> uJob = MakeUnique<Job>(++m_nextEngineJobID, std::move(task), std::move(name), false);
	return Lock_Enqueue(std::move(uJob), m_engineJobQueue);
}

MultiJob* JobManager::CreateMultiJob(String name)
{
	m_uMultiJobs.emplace_back(MakeUnique<MultiJob>(std::move(name)));
	return m_uMultiJobs.back().get();
}

s32 JobManager::AvailableEngineThreads() const
{
	s32 count = m_engineWorkers.size();
	for (auto& engineWorker : m_engineWorkers)
	{
		if (engineWorker->GetState() == JobWorker::State::WORKING)
		{
			--count;
		}
	}
	return count;
}

void JobManager::Tick(Time /*dt*/)
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

UPtr<JobManager::Job> JobManager::Lock_PopJob(bool bEngineQueue)
{
	Lock lock(m_mutex);
	List<UPtr<Job>>& jobList = bEngineQueue ? m_engineJobQueue : m_gameJobQueue;
	if (!jobList.empty())
	{
		UPtr<Job> uJob = std::move(jobList.back());
		jobList.pop_back();
		return uJob;
	}
	return {};
}

SPtr<JobHandle> JobManager::Lock_Enqueue(UPtr<Job>&& uJob, List<UPtr<Job>>& jobQueue)
{
	SPtr<JobHandle> sHandle = uJob->m_sHandle;
	Lock lock(m_mutex);
	jobQueue.emplace_front(std::move(uJob));
	return sHandle;
}
} // namespace LittleEngine
