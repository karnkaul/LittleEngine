#include "stdafx.h"
#include <thread>
#include "JobManager.h"
#include "Logger.h"
#include "Utils.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Engine/OS.h"

namespace LittleEngine
{
MultiJob::Job::Job(const String& name, Function(void()) job) : name(name), job(job)
{
}

MultiJob::MultiJob(const String& name) : m_name(name)
{
	LogD(LogName() + " created");
}

void MultiJob::AddJob(Function(void()) job, const String& name)
{
	String n = name;
	if (name.empty())
		n = "MultiJob_" + Strings::ToString(m_Jobs.size());
	m_Jobs.emplace_back(name, job);
}

void MultiJob::StartJobs(Function(void()) OnComplete)
{
	JobManager* pJobs = Services::Jobs();
	Assert(pJobs, "JobManager Service is null!");
	LogI(LogName() + " started");
	m_OnComplete = OnComplete;
	Vector<JobID> jobIDs;
	for (auto& job : m_Jobs)
	{
		jobIDs.push_back(pJobs->Enqueue(job.job, job.name));
	}
	pJobs->Enqueue(
		[&, jobIDs]() {
			for (auto id : jobIDs)
			{
				Services::Jobs()->Wait(id);
			}
			m_bCompleted = true;
		},
		"MultiJob Observer");
}

void MultiJob::Update()
{
	if (m_bCompleted && m_OnComplete)
	{
		LogD(LogName() + " completed");
		m_OnComplete();
		m_OnComplete = nullptr;
	}
}

String MultiJob::LogName() const
{
	return "[" + m_name + " (MultiJob)]";
}

JobManager::Job::Job(JobID id, Function(void()) task, String name) : name(name), task(task), id(id)
{
}

String JobManager::Job::ToString() const
{
	String nameStr = name.empty() ? "" : "-" + name;
	return "[" + Strings::ToString(id) + nameStr + "]";
}

JobManager::JobManager()
{
	u32 systemWorkers = OS::Platform()->SystemWorkerCount();
	u32 userWorkers = OS::Platform()->UserWorkerCount();
	m_availableSystemThreads = systemWorkers;
	for (u32 i = 0; i < userWorkers; ++i)
	{
		m_userWorkers.emplace_back(MakeUnique<JobWorker>(*this, i + 100, false));
	}
	for (u32 i = 0; i < systemWorkers; ++i)
	{
		m_systemWorkers.emplace_back(MakeUnique<JobWorker>(*this, i, true));
	}
	LogI("[JobManager] Detected [" + Strings::ToString(OS::Platform()->TotalThreadCount()) + "] available threads. " +
		 "Spawned [" + Strings::ToString(systemWorkers) + "] System JobWorkers " + "and [" +
		 Strings::ToString(userWorkers) + "] User JobWorkers");
	Services::ProvideJobManager(*this);
}

JobManager::~JobManager()
{
	Services::UnprovideJobManager(*this);
	for (auto& worker : m_userWorkers)
	{
		worker->Stop();
	}
	m_userWorkers.clear();
	for (auto& worker : m_systemWorkers)
	{
		worker->Stop();
	}
	m_systemWorkers.clear();
	LogI("[JobManager] destroyed");
}

void JobManager::Wait(JobID id)
{
	// Is Job in queue?
	bool bInQueue = false;
	do
	{
		{
			Lock lock(m_mutex);
			if (IsCompleted(id))
				return;
			bInQueue = IsEnqueued(id);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while (bInQueue);

	// Is Job running?
	JobWorker* pWorker = nullptr;
	{
		Lock lock(m_mutex);
		if (IsCompleted(id))
			return;
		if (id < 100)
			pWorker = GetSystemWorker(id);
		else
			pWorker = GetUserWorker(id);
	}
	if (pWorker)
		pWorker->Wait();
	else if (!IsCompleted(id))
		LogW("[JobManager] JobID [" + Strings::ToString(id) + "] not found!");
}

bool JobManager::IsRunning(JobID id)
{
	Lock lock(m_mutex);
	return GetUserWorker(id) || IsEnqueued(id);
}

JobID JobManager::Enqueue(Function(void()) Task, const String& name)
{
	JobID id = ++m_nextUserID;
	Job job(id, Task, name);
	{
		Lock lock(m_mutex);
		m_userJobQueue.emplace_front(std::move(job));
	}
	return id;
}

MultiJob* JobManager::CreateMultiJob(const String& name)
{
	UPtr<MultiJob> uMultiJob = MakeUnique<MultiJob>(name);
	MultiJob* pMultiJob = uMultiJob.get();
	m_uMultiJobs.emplace_back(std::move(uMultiJob));
	return pMultiJob;
}

s32 JobManager::AvailableSystemThreads() const
{
	s32 count = m_systemWorkers.size();
	for (auto& systemWorker : m_systemWorkers)
	{
		if (systemWorker->GetState() == JobWorker::State::WORKING)
			--count;
	}
	return count;
}

JobID JobManager::EnqueueSystem(Function(void()) Task, const String& name)
{
	Assert(AvailableSystemThreads() > 0, "!DEADLOCK! No available system workers!");
	JobID id = ++m_nextSystemID;
	Job job(id, Task, name);
	{
		Lock lock(m_mutex);
		m_systemJobQueue.emplace_front(std::move(job));
	}
	return id;
}

bool JobManager::IsCompleted(JobID id)
{
	return std::find(m_completed.begin(), m_completed.end(), id) != m_completed.end();
}

bool JobManager::IsEnqueued(JobID id)
{
	auto userIter = std::find_if(m_userJobQueue.begin(), m_userJobQueue.end(),
								 [id](Job& job) { return job.id == id; });
	auto systemIter = std::find_if(m_systemJobQueue.begin(), m_systemJobQueue.end(),
								   [id](Job& job) { return job.id == id; });
	return userIter != m_userJobQueue.end() || systemIter != m_systemJobQueue.end();
}

JobWorker* JobManager::GetUserWorker(JobID id)
{
	auto iter = std::find_if(m_userWorkers.begin(), m_userWorkers.end(),
							 [id](UPtr<JobWorker>& uJob) { return uJob->GetJobID() == id; });
	return (iter == m_userWorkers.end()) ? nullptr : iter->get();
}

JobWorker* JobManager::GetSystemWorker(JobID id)
{
	auto iter = std::find_if(m_systemWorkers.begin(), m_systemWorkers.end(),
							 [id](UPtr<JobWorker>& uJob) { return uJob->GetJobID() == id; });
	return (iter == m_systemWorkers.end()) ? nullptr : iter->get();
}

void JobManager::Tick(Time)
{
	auto iter = m_uMultiJobs.begin();
	while (iter != m_uMultiJobs.end())
	{
		(*iter)->Update();
		if ((*iter)->m_bCompleted)
		{
			LogD("[JobManager] " + (*iter)->LogName() + " completed. Destroying instance.");
			iter = m_uMultiJobs.erase(iter);
			continue;
		}
		++iter;
	}
}
} // namespace LittleEngine
