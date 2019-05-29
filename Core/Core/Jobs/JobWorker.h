#pragma once
#include <atomic>
#include "Core/CoreTypes.h"
#include "Core/OS.h"

namespace Core
{
class JobWorker final
{
public:
	enum class State
	{
		IDLE,
		WORKING,
	};

private:
	u8 id = 0;
	class JobManager* m_pManager = nullptr;
	OS::Threads::Handle m_threadHandle;
	std::atomic<bool> m_bWork;
	State m_state = State::IDLE;
	const bool m_bEngineWorker;

public:
	JobWorker(JobManager& manager, u8 id, bool bEngineWorker);
	~JobWorker();

	void Stop();
	State GetState() const;

private:
	String m_logName;
	void Run();
};
} // namespace LittleEngine
