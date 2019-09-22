#include "JobWorker.h"
#include "JobManager.h"
#include "Core/Logger.h"
#include "Core/OS.h"
#include "Core/Utils.h"

namespace Core
{
JobWorker::JobWorker(JobManager& manager, u8 id, bool bEngineWorker) : id(id), m_pManager(&manager), m_bEngineWorker(bEngineWorker)
{
	m_bWork.store(true, std::memory_order_relaxed);
	m_logName = "[JobWorker" + Strings::ToString(this->id) + "]";
	m_threadHandle = OS::Threads::Spawn([&]() { Run(); });
}

JobWorker::~JobWorker()
{
	OS::Threads::Join(m_threadHandle);
	LOG_D("%s destroyed", m_logName.c_str());
}

void JobWorker::Stop()
{
	m_bWork.store(false, std::memory_order_relaxed);
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
		m_state = State::Idle;

		auto oJob = m_pManager->Lock_PopJob();
		if (!oJob)
		{
			std::this_thread::yield();
		}

		else
		{
			m_state = State::Busy;

			String suffix = m_bEngineWorker ? " Engine Job " : " Job ";
			if (!oJob->m_bSilent)
			{
				LOG_D("%s Starting %s %s", m_logName.c_str(), m_bEngineWorker ? "Engine Job" : "Job", oJob->ToStr());
			}
			oJob->Run();
			if (!oJob->m_bSilent && !oJob->m_szException)
			{
				LOG_D("%s Completed %s %s", m_logName.c_str(), m_bEngineWorker ? "Engine Job" : "Job", oJob->ToStr());
			}
			if (oJob->m_szException)
			{
				LOG_E("%s Threw an exception running %s\n\t%s!", m_logName.c_str(), oJob->ToStr(), oJob->m_szException);
			}
			oJob->Fulfil();
		}
	}
}
} // namespace Core
