#pragma once
#include <atomic>
#include <thread>
#include "CoreTypes.h"

namespace LittleEngine
{
using JobID = s32;

class JobWorker final
{
public:
	enum class State
	{
		IDLE,
		WORKING,
	};

private:
	u32 id;
	class JobManager* m_pManager;
	std::thread m_thread;
	std::atomic<bool> m_bWork = true;
	State m_state;
	JobID m_jobID;
	const bool m_bSystemWorker;

public:
	JobWorker(JobManager& manager, u32 id, bool bSystem);
	~JobWorker();

	void Stop();
	void Wait();
	JobID GetJobID() const;
	State GetState() const;

private:
	String LogName() const;
	void Run();
};
} // namespace LittleEngine
