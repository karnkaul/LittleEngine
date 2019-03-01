#pragma once
#include <atomic>
#include <mutex>
#include <functional>
#include "CoreTypes.h"
#include "JobWorker.h"
#include "MultiJob.h"
#include "LittleEngine/Services/IService.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
using JobID = s32;

class JobManager final : public IService
{
public:
	static constexpr JobID INVALID_ID = -1;

private:
	using Lock = std::lock_guard<std::mutex>;

	struct Job
	{
		String logName;
		std::function<void()> task;
		JobID id = INVALID_ID;
		bool bSilent = false;

		Job() = default;
		Job(JobID id, const std::function<void()>& task, String name, bool bSilent);

		const char* ToStr() const;
	};

private:
	Vec<UPtr<class JobWorker>> m_gameWorkers;
	Vec<UPtr<class JobWorker>> m_engineWorkers;
	Vec<JobID> m_completed;
	List<UPtr<MultiJob>> m_uMultiJobs;
	List<Job> m_gameJobQueue;
	List<Job> m_engineJobQueue;
	std::mutex m_mutex;
	JobID m_nextGameJobID = 100;
	JobID m_nextEngineJobID = 0;
	u32 m_availableEngineThreads;

public:
	JobManager();
	~JobManager();

	void Wait(JobID id);
	void Wait(InitList<JobID> ids);
	void Wait(const Vec<JobID>& ids);
	bool IsRunning(JobID id);
	bool IsCompleted(JobID id);

public:
	JobID Enqueue(const std::function<void()>& Task, const String& name = "", bool bSilent = false);
	JobID EnqueueEngine(const std::function<void()>& Task, const String& name);
	MultiJob* CreateMultiJob(const String& name);

private:
	s32 AvailableEngineThreads() const;
	void Tick(Time dt);

	bool Unsafe_IsCompleted(JobID id);
	bool Unsafe_IsEnqueued(JobID id);
	JobWorker* Unsafe_GetGameWorker(JobID id);
	JobWorker* Unsafe_GetEngineWorker(JobID id);

	Job Lock_PopJob(bool bEngineQueue = false);
	void Lock_CompleteJob(JobID id);
	JobID Lock_Enqueue(Job&& job, List<Job>& jobQueue);

	friend class EngineService;
	friend class EngineLoop;
	friend class JobWorker;
};
} // namespace LittleEngine
