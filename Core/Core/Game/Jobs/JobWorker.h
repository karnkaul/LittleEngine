#pragma once
#include <atomic>
#include "../GameTypes/StdTypes.h"
#include "../OS.h"

namespace Core
{
class JobWorker final
{
public:
	enum class State : u8
	{
		Idle,
		Busy,
	};

private:
	u8 id = 0;
	class JobManager* m_pManager = nullptr;
	OS::Threads::Handle m_threadHandle;
	std::atomic<bool> m_bWork;
	State m_state = State::Idle;
	const bool m_bEngineWorker;

public:
	JobWorker(JobManager& manager, u8 id, bool bEngineWorker);
	~JobWorker();

	void Stop();
	State GetState() const;

private:
	std::string m_logName;
	void Run();
};
} // namespace Core
