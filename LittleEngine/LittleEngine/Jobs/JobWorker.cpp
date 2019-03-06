#include "stdafx.h"
#include "JobWorker.h"
#include "JobManager.h"
#include "Logger.h"
#include "Utils.h"

namespace LittleEngine
{
JobWorker::JobWorker(JobManager& manager, u8 id, bool bEngineWorker)
	: id(id), m_pManager(&manager), m_bEngineWorker(bEngineWorker)
{
	m_bWork.store(true, std::memory_order_relaxed);
	m_logName = "[JobWorker" + Strings::ToString(id) + "]";
	m_thread = std::thread(std::bind(&JobWorker::Run, this));
}

JobWorker::~JobWorker()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	LOG_D("%s destroyed", m_logName.c_str());
}

void JobWorker::Stop()
{
	m_bWork.store(false, std::memory_order_relaxed);
}

void JobWorker::Wait()
{
	while (m_state == State::WORKING)
	{
		std::this_thread::yield();
	}
}

JobHandle JobWorker::GetJobID() const
{
	return m_jobID;
}

JobWorker::State JobWorker::GetState() const
{
	return m_state;
}

void JobWorker::Run()
{
	while (m_bWork.load(std::memory_order_relaxed))
	{
		// Reset
		m_state = State::IDLE;
		m_jobID = JobManager::INVALID_ID;

		// Obtain job
		JobManager::Job job = m_pManager->Lock_PopJob(m_bEngineWorker);

		// No jobs in queue
		if (job.id == JobManager::INVALID_ID)
		{
			std::this_thread::yield();
		}

		// Perform job
		else
		{
			m_state = State::WORKING;
			m_jobID = job.id;
			String suffix = m_bEngineWorker ? " Engine Job " : " Job ";
			if (!job.bSilent)
				LOG_D("%s Starting %s %s", m_logName.c_str(), m_bEngineWorker ? "Engine Job" : "Job", job.ToStr());
			try
			{
				job.task();
			}
			catch (const std::exception& e)
			{
				Assert(false, "AsyncTask threw exception!");
				LOG_E("%d threw an exception and job has failed! %d\n%s", m_jobID, e.what());
			}
			m_pManager->Lock_CompleteJob(m_jobID);
			if (!job.bSilent)
				LOG_D("%s Completed %s %s", m_logName.c_str(), m_bEngineWorker ? "Engine Job" : "Job", job.ToStr());
		}
	}
}
} // namespace LittleEngine
