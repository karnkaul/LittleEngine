#include "stdafx.h"
#include <thread>
#include "JobManager.h"
#include "Logger.h"
#include "Utils.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Engine/OS.h"

namespace LittleEngine
{
JobManager::Job::Job(JobID id, const std::function<void()>& task, String name, bool bSilent)
	: task(task), id(id), bSilent(bSilent)
{
	String suffix = name.empty() ? "" : "-" + name;
	logName = "[" + Strings::ToString(id) + suffix + "]";
}

const char* JobManager::Job::ToStr() const
{
	return logName.c_str();
}

JobManager::JobManager()
{
	u32 engineWorkers = OS::Platform()->SystemWorkerCount();
	u32 gameWorkers = OS::Platform()->UserWorkerCount();
	m_availableEngineThreads = engineWorkers;
	for (u32 i = 0; i < gameWorkers; ++i)
	{
		m_gameWorkers.emplace_back(MakeUnique<JobWorker>(*this, i + 100, false));
	}
	for (u32 i = 0; i < engineWorkers; ++i)
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

void JobManager::Wait(JobID id)
{
	// Is Job in queue?
	bool bInQueue = false;
	do
	{
		{
			Lock lock(m_mutex);
			if (Unsafe_IsCompleted(id))
				return;
			bInQueue = Unsafe_IsEnqueued(id);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while (bInQueue);

	// Is Job running?
	JobWorker* pWorker = nullptr;
	{
		Lock lock(m_mutex);
		if (Unsafe_IsCompleted(id))
			return;
		pWorker = id < 100 ? Unsafe_GetEngineWorker(id) : Unsafe_GetGameWorker(id);
	}
	if (pWorker)
	{
		pWorker->Wait();
	}
#if DEBUGGING
	else if (!IsCompleted(id))
	{
		LOG_E("[JobManager] JobID [%d] not found!", id);
	}
#endif
}

void JobManager::Wait(InitList<JobID> ids)
{
	for (auto id : ids)
	{
		Wait(id);
	}
}

void JobManager::Wait(const Vector<JobID>& ids)
{
	for (auto id : ids)
	{
		Wait(id);
	}
}

bool JobManager::IsRunning(JobID id)
{
	Lock lock(m_mutex);
	return Unsafe_IsEnqueued(id) || Unsafe_GetGameWorker(id);
}

bool JobManager::IsCompleted(JobID id)
{
	Lock lock(m_mutex);
	return Unsafe_IsCompleted(id);
}

JobID JobManager::Enqueue(const std::function<void()>& Task, const String& name, bool bSilent)
{
	Job job(++m_nextGameJobID, Task, name, bSilent);
	return Lock_Enqueue(std::move(job), m_gameJobQueue);
}

JobID JobManager::EnqueueEngine(const std::function<void()>& Task, const String& name)
{
	Assert(AvailableEngineThreads() > 0, "!DEADLOCK! No available engine workers!");
	Job job(++m_nextEngineJobID, Task, name, false);
	return Lock_Enqueue(std::move(job), m_engineJobQueue);
}

MultiJob* JobManager::CreateMultiJob(const String& name)
{
	m_uMultiJobs.emplace_back(MakeUnique<MultiJob>(name));
	return m_uMultiJobs.back().get();
}

s32 JobManager::AvailableEngineThreads() const
{
	s32 count = m_engineWorkers.size();
	for (auto& engineWorker : m_engineWorkers)
	{
		if (engineWorker->GetState() == JobWorker::State::WORKING)
			--count;
	}
	return count;
}

void JobManager::Tick(Time)
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

bool JobManager::Unsafe_IsCompleted(JobID id)
{
	return std::find(m_completed.begin(), m_completed.end(), id) != m_completed.end();
}

bool JobManager::Unsafe_IsEnqueued(JobID id)
{
	auto gameIter = std::find_if(m_gameJobQueue.begin(), m_gameJobQueue.end(),
								 [id](Job& job) { return job.id == id; });
	auto engineIter = std::find_if(m_engineJobQueue.begin(), m_engineJobQueue.end(),
								   [id](Job& job) { return job.id == id; });
	return gameIter != m_gameJobQueue.end() || engineIter != m_engineJobQueue.end();
}

JobWorker* JobManager::Unsafe_GetGameWorker(JobID id)
{
	auto iter = std::find_if(m_gameWorkers.begin(), m_gameWorkers.end(),
							 [id](UPtr<JobWorker>& uJob) { return uJob->GetJobID() == id; });
	return (iter == m_gameWorkers.end()) ? nullptr : iter->get();
}

JobWorker* JobManager::Unsafe_GetEngineWorker(JobID id)
{
	auto iter = std::find_if(m_engineWorkers.begin(), m_engineWorkers.end(),
							 [id](UPtr<JobWorker>& uJob) { return uJob->GetJobID() == id; });
	return (iter == m_engineWorkers.end()) ? nullptr : iter->get();
}

JobManager::Job JobManager::Lock_PopJob(bool bEngineJob)
{
	Lock lock(m_mutex);
	List<JobManager::Job>& jobList = bEngineJob ? m_engineJobQueue : m_gameJobQueue;
	if (!jobList.empty())
	{
		Job job = std::move(jobList.back());
		jobList.pop_back();
		return job;
	}
	return {};
}

void JobManager::Lock_CompleteJob(JobID id)
{
	Lock lock(m_mutex);
	m_completed.push_back(id);
}

JobID JobManager::Lock_Enqueue(Job&& job, List<Job>& jobQueue)
{
	JobID id = job.id;
	Lock lock(m_mutex);
	jobQueue.emplace_front(std::move(job));
	return id;
}
} // namespace LittleEngine
